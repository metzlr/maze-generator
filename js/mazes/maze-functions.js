const vsSource = `
  attribute vec4 a_position;
  attribute vec3 a_normal;
  //attribute vec4 a_color;

  uniform mat4 u_worldViewProjection;
  uniform mat4 u_worldInverseTranspose;

  varying vec3 v_normal;
  //varying vec4 v_color;

  void main() {
    // Multiply position by matrix
    gl_Position = u_worldViewProjection * a_position;

    // Pass the normal to the fragment shader
    v_normal = mat3(u_worldInverseTranspose) * a_normal;

    // // Pass color to fragment shader
    // v_color = a_color;
  }
`;
const fsSource = `
  precision mediump float;

  varying vec3 v_normal;
  // varying vec4 v_color;

  uniform vec3 u_reverseLightDirection;
  uniform vec4 u_color;

  void main() {
    // because v_normal is a varying it's interpolated
    // so it will not be a unit vector. Normalizing it
    // will make it a unit vector again
    vec3 normal = normalize(v_normal);

    float dirLightColor = 0.8;
    float ambientLightColor = 0.2;

    float light = dot(normal, u_reverseLightDirection) * dirLightColor + ambientLightColor;

    gl_FragColor = u_color;

    // Multiply color (excluding alpha) by light
    gl_FragColor.rgb *= light;

    //gl_FragColor = vec4(0.5 + 2.0 * v_normal, 1);
  }
`;

function radToDeg(r) {
  return r * 180 / Math.PI;
}

function degToRad(d) {
  return d * Math.PI / 180;
}

function createShader(gl, type, source) {
  var shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
  if (success) {
    return shader;
  }

  console.log("Failed to create shader\n" + gl.getShaderInfoLog(shader));
  gl.deleteShader(shader);
}

function createProgram(gl, vertexShader, fragmentShader) {
  var program = gl.createProgram();
  gl.attachShader(program, vertexShader);
  gl.attachShader(program, fragmentShader);
  gl.linkProgram(program);
  var success = gl.getProgramParameter(program, gl.LINK_STATUS);
  if (success) {
    return program;
  }
  console.log("Failed to create program\n" + gl.getProgramInfoLog(program));
  gl.deleteProgram(program);
}

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min)) + min; //The maximum is exclusive and the minimum is inclusive
}

var mazeFunctions = (function() {
  "use strict";
  function createMazeContainer(parent, generateFunction) {
    var mazeContainer = document.createElement("div");
    mazeContainer.className = "maze-container";
    mazeContainer.innerHTML = `
      <div class="flex-col-centered configure-maze-container"></div>
      <button class="generate-maze-button">Generate Maze</button>
      <div class="maze-canvas-container">
        <canvas class="maze-canvas"></canvas>
        <div class="canvas-checkbox-container flex-row-centered"></div>
      </div>
    `;
    var button = mazeContainer.querySelector('.generate-maze-button');
    button.addEventListener('click', generateFunction);
    parent.appendChild(mazeContainer);
    return mazeContainer;
  }

  function createMazeControlRow(elements) {
    var row = document.createElement("div");
    row.className = "flex-row-centered";
    elements.forEach((item) => {
      row.appendChild(item);
    });
    return row;
  }

  function drawMeshMaze2D(maze) {
    //console.log("Drawing maze")
    // Maze background
    canvasFunctions.resizeCanvas(maze.canvas);
    maze.ctx.fillStyle = '#f0f0f0';
    maze.ctx.fillRect(0, 0, maze.canvas.width, maze.canvas.height);
    // Calculate scale/offset values
    const minCanvas = (maze.canvas.width < maze.canvas.height) ? maze.canvas.width : maze.canvas.height;
    const max = (maze.size.x > maze.size.y) ? maze.size.x : maze.size.y;
    const offset = 1/20 * minCanvas;
    const scale = (minCanvas - 2*offset)/max;
    const xOffset = (maze.canvas.width - (scale * maze.size.x))/2;
    const yOffset = (maze.canvas.height - (scale * maze.size.y))/2;
    const shapeColor = '#316fed';
    const triangleColor = '#303030';
    const solutionColor = '#f72020';

    for (var i = 0; i < maze.objects.triangles.length; i++) {
      var v1 = maze.objects.vertices[maze.objects.triangles[i].indices[0]];
      var v2 = maze.objects.vertices[maze.objects.triangles[i].indices[1]];
      var v3 = maze.objects.vertices[maze.objects.triangles[i].indices[2]];
      maze.ctx.beginPath();
      maze.ctx.moveTo(v1.x * scale + xOffset, v1.y * scale + yOffset);
      maze.ctx.lineTo(v2.x * scale + xOffset, v2.y * scale + yOffset);
      maze.ctx.lineTo(v3.x * scale + xOffset, v3.y * scale + yOffset);
      maze.ctx.lineTo(v1.x * scale + xOffset, v1.y * scale + yOffset);
      maze.ctx.fillStyle = shapeColor;
      maze.ctx.fill();
      if (!maze.canvasSettings.showTriangles) {
        maze.ctx.strokeStyle = shapeColor;
        maze.ctx.lineJoin = 'bevel';
        maze.ctx.lineCap = 'butt';
        maze.ctx.lineWidth = 0.8;
      } else {
        maze.ctx.strokeStyle = triangleColor;
        maze.ctx.lineJoin = 'round';
        maze.ctx.lineCap = 'round';
        maze.ctx.lineWidth = 0.06*scale;
      }
      maze.ctx.stroke();
    }
    if (maze.canvasSettings.showSolution) {
      if (maze.objects.solution.length > 0) {
        maze.ctx.beginPath();
        maze.ctx.moveTo(maze.objects.solution[0].x * scale + xOffset, maze.objects.solution[0].y * scale + yOffset);
        for (var i = 1; i < maze.objects.solution.length; i++) {
          maze.ctx.lineTo(maze.objects.solution[i].x * scale + xOffset, maze.objects.solution[i].y * scale + yOffset);
        }
        maze.ctx.strokeStyle = solutionColor;
        maze.ctx.lineWidth = (0.3 * scale) + (0.3 * scale * parseFloat(maze.inputs.radius));
        maze.ctx.lineJoin = 'round';
        maze.ctx.lineCap = 'round';
        maze.ctx.line
        maze.ctx.stroke();
      }
    }
  };

  function parseMeshMaze2D(mazeData, maze) {
    //console.log("Parsing maze data")
    maze.seed = undefined;
    maze.size.x = undefined;
    maze.size.y = undefined;
    maze.objects.triangles = [];
    maze.objects.vertices = [];
    maze.objects.solution = [];
    var lines = mazeData.split("\n");
    for (var i = 0; i < lines.length; i++) {
      var line = lines[i].split("\t");
      if (line[0] === "seed") {
        if (line.length != 2) throw "Error: Invalid seed format"; 
        maze.seed = line[1];
        continue;
      } else if (line[0] === "") continue;
      var line = lines[i].split("\t");
      if (line[0] == "vertex") {
        var x_ = parseFloat(line[2]);
        var y_ = parseFloat(line[3]);
        maze.objects.vertices[parseInt(line[1])] = { x: x_, y: y_ };
        if (maze.size.x == undefined || x_ > maze.size.x) maze.size.x = x_;
        if (maze.size.y == undefined || y_ > maze.size.y) maze.size.y = y_;
      } else if (line[0] == "triangle") {
        maze.objects.triangles.push({
          indices: [
            parseFloat(line[1]),
            parseFloat(line[2]),
            parseFloat(line[3])
          ]
        });
      } else if (line[0] == "path") {
        var x_ = parseFloat(line[1]);
        var y_ = parseFloat(line[2]);
        maze.objects.solution.push({ x: x_, y: y_ });
      } else {
        console.log("Error parsing maze data:", line);
      }
    }
  };

  function drawBasicMaze(maze) {
    //console.log("Drawing maze");
    // Maze background
    canvasFunctions.resizeCanvas(maze.canvas);
    maze.ctx.fillStyle = '#f0f0f0';
    maze.ctx.fillRect(0, 0, maze.canvas.width, maze.canvas.height);
    // Calculate scale/offset values
    const drawVertices = false;
    const drawShapes = false;
    const drawEdges = true;
    const minCanvas = (maze.canvas.width < maze.canvas.height) ? maze.canvas.width : maze.canvas.height;
    const max = (maze.size.x > maze.size.y) ? maze.size.x : maze.size.y;
    const offset = 1/20 * minCanvas;
    const scale = (minCanvas - 2*offset)/max;
    const xOffset = (maze.canvas.width - (scale * maze.size.x))/2;
    const yOffset = (maze.canvas.height - (scale * maze.size.y))/2;
    const shapeColor = '#426cf5';
    const edgeColor = '#303030';
    const vertexColor = '#303030';
    const edgeWidth = 0.15*scale;
    // Draw to canvas
    if (drawShapes) {
      for (var i = 0; i < maze.objects.squares.length; i++) {
        var v1 = maze.objects.vertices[maze.objects.squares[i].vertexList[0]];
        var v2 = maze.objects.vertices[maze.objects.squares[i].vertexList[2]];
        var cushion = 0.5;
        var w = (v2.x - v1.x) * scale + cushion*2;
        var h = (v2.y - v1.y) * scale + cushion*2;
        var x = xOffset + v1.x * scale - cushion;
        var y = yOffset + v1.y * scale - cushion;
        
        maze.ctx.fillStyle = shapeColor;
        maze.ctx.fillRect(x, y, w, h);
      }
    }
    if (drawEdges) {
      for (var i = 0; i < maze.objects.edges.length; i++) {
        if (!maze.objects.edges[i].visible) continue;
        var v1 = maze.objects.vertices[maze.objects.edges[i].vertexList[0]];
        var v2 = maze.objects.vertices[maze.objects.edges[i].vertexList[1]];
        maze.ctx.beginPath();
        maze.ctx.moveTo(xOffset + v1.x * scale, yOffset + v1.y * scale);
        maze.ctx.lineTo(xOffset + v2.x * scale, yOffset + v2.y * scale);
        maze.ctx.strokeStyle = edgeColor;
        maze.ctx.lineWidth = edgeWidth;
        maze.ctx.lineCap = 'round';
        maze.ctx.stroke();
      }
    }
    if (drawVertices) {
      for (var i = 0; i < maze.objects.vertices.length; i++) {
        var x = xOffset + maze.objects.vertices[i].x * scale;// - edgeWidth/2;
        var y = yOffset + maze.objects.vertices[i].y * scale;// - edgeWidth/2;
        // var w = edgeWidth;
        // var h = edgeWidth;
        var radius = edgeWidth;
        maze.ctx.fillStyle = edgeColor;
        maze.ctx.beginPath();
        maze.ctx.arc(x, y, radius, 0, 2 * Math.PI);
        maze.ctx.fill();
        //maze.ctx.fillRect(x,y,w,h);
      }
    }
  };

  function parseBasicMaze(mazeData, maze) {
    //console.log("Parsing maze data");
    mazeData = mazeData.toString();
    maze.seed = undefined;
    maze.objects.vertices = [];
    maze.objects.edges = [];
    maze.objects.squares = [];
    maze.size.x = undefined;
    maze.size.y = undefined;
    var lines = mazeData.split("\n");
    for (var i = 0; i < lines.length; i++) {
      var line = lines[i].split("\t");
      if (line[0] === "seed") {
        if (line.length != 2) throw "Error: Invalid seed format"; 
        maze.seed = line[1];
      } else if (line[0] === "vertex") {
        if (line.length != 4) throw "Error: Invalid vertex format";
        var id = parseInt(line[1], 10);
        if (isNaN(id)) throw "Error: Invalid vertex ID";
        var posX = parseFloat(line[2]);
        var posY = parseFloat(line[3]);
        if (isNaN(posX) || isNaN(posY)) throw "Error: Invalid vertex position";
        maze.objects.vertices[id] = { 
          x : posX,
          y : posY
        };
        if (maze.size.x == undefined || posX > maze.size.x) maze.size.x = posX;
        if (maze.size.y == undefined || posY > maze.size.y) maze.size.y = posY;
      } else if (line[0] === "square") {
        if (line.length != 6) throw "Error: Invalid square format";
        var id = parseInt(line[1], 10);
        if (isNaN(id)) throw "Error: Invalid square ID";
        maze.objects.squares[id] = { vertexList : [] }
        for (var v = 0; v < 4; v++) {
          var vID = parseInt(line[v+2], 10);
          if (isNaN(vID)) throw "Error: Invalid vertex ID for square";
          maze.objects.squares[id].vertexList[v] = vID;
        }
      } else if (line[0] === "edge") {
        if (line.length != 5) throw "Error: Invalid edge format";
        var id = parseInt(line[1], 10);
        if (isNaN(id)) throw "Error: Invalid edge ID";
        maze.objects.edges[id] = { vertexList : [] }
        for (var v = 0; v < 2; v++) {
          var vID = parseInt(line[v+2], 10);
          if (isNaN(vID)) throw "Error: Invalid vertex ID for edge";
          maze.objects.edges[id].vertexList[v] = vID;
        }
        var visible = parseInt(line[4], 10);
        if (isNaN(visible)) throw "Error: Invalid edge visibility value";
        maze.objects.edges[id].visible = (visible === 0 ? false : true);
      } else {
        if (line[0] == "") continue;
        console.log(line);
        throw ("Error: Unknown data in maze file")
      }
    }
  };

  function parseMeshMaze3D(mazeData, maze) {
    maze.seed = undefined;
    maze.size.x = undefined;
    maze.size.y = undefined;
    maze.size.z = undefined;
    var vertexList = [];
    maze.objects.vertices = [];
    maze.objects.indices = [];
    maze.objects.normals = [];
    var lines = mazeData.split("\n");
    for (var i = 0; i < lines.length; i++) {
      var line = lines[i].split("\t");
      if (line[0] === "seed") {
        if (line.length != 2) throw "Error: Invalid seed format"; 
        maze.seed = line[1];
        //console.log(maze.seed);
        continue;
      } else if (line[0] === "") continue;
      var line = lines[i].split("\t");
      if (line[0] == "vertex") {
        var x_ = parseFloat(line[2]);
        var y_ = parseFloat(line[3]);
        var z_ = parseFloat(line[4]);
        vertexList[parseInt(line[1])] = { 
          point: [x_, y_, z_],
          normals: [ parseFloat(line[5]), parseFloat(line[6]), parseFloat(line[7]) ]
        };
        if (maze.size.x == undefined || x_ > maze.size.x) maze.size.x = x_;
        if (maze.size.y == undefined || y_ > maze.size.y) maze.size.y = y_;
        if (maze.size.z == undefined || z_ > maze.size.z) maze.size.z = z_;
      } else if (line[0] == "triangle") {
        maze.objects.indices.push(parseFloat(line[1]), parseFloat(line[2]), parseFloat(line[3]));
      } else {
        console.log("Error parsing maze data:", line);
      }
    }
    // Move vertices/normals into 1D array for easy conversion to buffer
    for (var i = 0; i < vertexList.length; i++) {
      maze.objects.vertices.push(vertexList[i].point[0], vertexList[i].point[1], vertexList[i].point[2]);
      maze.objects.normals.push(vertexList[i].normals[0], vertexList[i].normals[1], vertexList[i].normals[2]);
    }
  };

  function drawMeshMaze3D(maze) {
    var gl = maze.gl;
    canvasFunctions.resizeCanvas(gl.canvas); // resize canvas
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height); // Map WebGL clip space to canvas size

    // Clear canvas
    //gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Turn on culling. By default backfacing triangles will be culled.
    gl.enable(gl.CULL_FACE);

    // Enable the depth buffer
    gl.enable(gl.DEPTH_TEST);
    
    gl.useProgram(maze.glData.program); 

    // SETUP ATTRIBUTES
    // Turn on attribute
    gl.enableVertexAttribArray(maze.glData.attributeLocations.positionLocation);
    // Bind position buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, maze.glData.buffers.positionBuffer);
    // Tell attribute how to get data from position buffer
    var size = 3;           // 3 compoments per iteration (x,y,z)
    var type = gl.FLOAT     // data type (32bit float)
    var normalize = false;  // dont normalize data
    var stride = 0;         // 0 = move forward size * sizeof(type) each iteration
    var offset = 0;         // start at beginning of the buffer
    gl.vertexAttribPointer(maze.glData.attributeLocations.positionLocation, size, type, normalize, stride, offset); // Also binds current ARRAY_BUFFER, so it can now be rebinded

    // Turn on normal attribute
    gl.enableVertexAttribArray(maze.glData.attributeLocations.normalLocation);
    // Bind normal buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, maze.glData.buffers.normalBuffer);
    // Tell attribute how to get data from normal buffer
    var size = 3;
    var type = gl.FLOAT;
    var normalize = false;
    var stride = 0;
    var offset = 0;
    gl.vertexAttribPointer(maze.glData.attributeLocations.normalLocation, size, type, normalize, stride, offset);

    // Compute the projection matrix
    var aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
    var zNear = 1;
    var zFar = 2000;
    var projectionMatrix = m4.perspective(maze.state.fieldOfViewRadians, aspect, zNear, zFar);
  
    // Compute camera matrix
    var cameraPosition = [0, 0, 50];
    var target = [0, 0, 0];
    var up = [0, 1, 0];
    var cameraMatrix = m4.lookAt(cameraPosition, target, up);

    // Make view matrix from camera matrix (view matrix is matrix that moves everything relative to the camera)
    var viewMatrix = m4.inverse(cameraMatrix);

    // Compute a view projection matrix
    var viewProjectionMatrix = m4.multiply(projectionMatrix, viewMatrix);

    // SETUP UNIFROMS
    var worldMatrix = m4.translation(maze.state.translation.x, maze.state.translation.y, maze.state.translation.z);
    
    worldMatrix = m4.xRotate(worldMatrix, maze.state.angle.x);
    worldMatrix = m4.yRotate(worldMatrix, maze.state.angle.y);
    worldMatrix = m4.zRotate(worldMatrix, maze.state.angle.z);
    worldMatrix = m4.scale(worldMatrix, maze.state.scale, maze.state.scale, maze.state.scale);
    // Offset origin point
    var offsetMatrix = m4.translation(-maze.size.x/2, -maze.size.y/2, -maze.size.z/2);
    worldMatrix = m4.multiply(worldMatrix, offsetMatrix);

    var worldViewProjectionMatrix = m4.multiply(viewProjectionMatrix, worldMatrix);
    var worldInverseMatrix = m4.inverse(worldMatrix);
    var worldInverseTranspose = m4.transpose(worldInverseMatrix);

    // Set the matrix
    gl.uniformMatrix4fv(maze.glData.uniformLocations.worldViewProjectionLocation, false, worldViewProjectionMatrix);
    gl.uniformMatrix4fv(maze.glData.uniformLocations.worldInverseTransposeLocation, false, worldInverseTranspose);
    // Set the color to use
    gl.uniform4fv(maze.glData.uniformLocations.colorLocation, [0.192, 0.435, 0.929, 1]); 
    // Set the light direction
    gl.uniform3fv(maze.glData.uniformLocations.reverseLightDirectionLocation, m4.normalize([0, 0, 1]));

    // DRAW GEOMETRY
    var primitiveType = gl.TRIANGLES; // What will be drawn
    var offset = 0;
    var count = maze.objects.indices.length;
    var indexType = gl.UNSIGNED_INT;
    gl.drawElements(primitiveType, count, indexType, offset);
  }

  return {
    createMazeContainer: createMazeContainer,
    drawMeshMaze2D: drawMeshMaze2D,
    parseMeshMaze2D: parseMeshMaze2D,
    drawBasicMaze: drawBasicMaze,
    parseBasicMaze: parseBasicMaze,
    parseMeshMaze3D: parseMeshMaze3D,
    drawMeshMaze3D: drawMeshMaze3D,
    createMazeControlRow: createMazeControlRow,
  }
})();
