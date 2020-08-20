
class Maze {
  constructor(parentContainer, endpointURL, generateFunction, defaultInputs, context) {
    this.container = mazeFunctions.createMazeContainer(parentContainer, generateFunction);
    this.canvas = this.container.querySelector("canvas");
    if (context === '2d') {
      this.ctx = this.canvas.getContext('2d')
    } else {
      this.gl = this.canvas.getContext('webgl');
    }
    this.endpointURL = endpointURL;
    //this.baseURL = 'http://localhost:8000';
    this.baseURL = 'http://underinformed.net';
    this.seed = undefined;
    this.loading = false;
    this.size = {
      x: undefined,
      y: undefined,
      z: undefined,
    }
    this.objects = {};
    this.inputs = defaultInputs;
    this.canvasSettings = {};
    
  }

  addControls(controls) {
    controls.forEach((row) => {
      var rowContainer = mazeFunctions.createMazeControlRow(row);
      this.container.querySelector('.configure-maze-container').appendChild(rowContainer);
    });
  }

  addCanvasCheckbox(control) {
    this.container.querySelector('.canvas-checkbox-container').appendChild(control);
  }

  fetchMaze() {
    var url = new URL(this.endpointURL, this.baseURL);
    url.search = new URLSearchParams(this.inputs).toString();
    return fetch(url).then(response => { return response.json(); });
  }
}



var maze1 = (function() {
  "use strict";
  function getNewMaze() {
    if (!maze.loading) {
      maze.loading = true;
      maze.fetchMaze().then((data) => {
        mazeFunctions.parseBasicMaze(data, maze);
        mazeFunctions.drawBasicMaze(maze);
        maze.loading = false;
      })
    }
  };
  var maze = new Maze(document.querySelector("#first-maze-container"), '/maze_generator.php', getNewMaze, 
    {
      "width": 20,
      "height": 20,
    }, 
    '2d',
  );
  
  var widthSlider = customUI.createCustomSlider("width", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Width", slide: ((event, newValue) => {
      maze.inputs["width"] = newValue;
    }),
  });
  var heightSlider = customUI.createCustomSlider("height", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Height", slide: ((event, newValue) => {
      maze.inputs["height"] = newValue;
    }),
  });

  maze.addControls([ [widthSlider, heightSlider] ]);
  getNewMaze();
  
})();

var maze2 = (function() {
  "use strict";

  function getNewMaze() {
    if (!maze.loading) {
      maze.loading = true;
      maze.fetchMaze().then((data) => {
        mazeFunctions.parseMeshMaze2D(data, maze);
        mazeFunctions.drawMeshMaze2D(maze);
        maze.loading = false;
      })
    }
  };

  var maze = new Maze(
    document.querySelector("#second-maze-container"), '/maze_generator2.php', getNewMaze,
    {
      "width": 20,
      "height": 20,
      "resolution": 1,
      "radius": 0,
      "startType": "edge",
      "centerX": 1,
      "centerY": 1,
      "blocky": false,
    },
    '2d',
  );
  maze.addCanvasCheckbox(canvasFunctions.createCanvasCheckbox("showTriangles", { value: false, type: "checkbox", text: "Show Triangles", change: (event, value) => {
      maze.canvasSettings['showTriangles'] = value;
      mazeFunctions.drawMeshMaze2D(maze);
    }
  }));
  maze.addCanvasCheckbox(canvasFunctions.createCanvasCheckbox("showSolution", { value: false, type: "checkbox", text: "Show Solution", change: (event, value) => {
      maze.canvasSettings['showSolution'] = value;
      mazeFunctions.drawMeshMaze2D(maze);
    }
  }));
  var startTypeSelector = customUI.createCustomSelect("startType", {
    text: "Start Location",
    options: [
      { text: "Edge", value: "edge" }, { text: "Center", value: "center" },
    ],
    change: ((event, value) => {
      maze.inputs["startType"] = value;
      var centerSliderX = maze.container.querySelector(".slider-container[name='centerX']");
      var centerSliderY = maze.container.querySelector(".slider-container[name='centerY']");
      if (value == "edge") {
        centerSliderX.querySelector('input[type="range"]').disabled = true;
        centerSliderY.querySelector('input[type="range"]').disabled = true;
        centerSliderX.style.opacity = 0.25;
        centerSliderY.style.opacity = 0.25;
      } else {
        centerSliderX.querySelector('input[type="range"]').disabled = false;
        centerSliderY.querySelector('input[type="range"]').disabled = false;
        centerSliderX.style.opacity = 1;
        centerSliderY.style.opacity = 1;
      }
    }),
  });
  var controls = [
    [
      customUI.createCustomSlider("width", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Width", slide: (event, newValue) => {
          maze.inputs["width"] = newValue;
        },
      }),
      customUI.createCustomSlider("height", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Height", slide: (event, newValue) => {
          maze.inputs["height"] = newValue;
        },
      }),
    ],
    [
      customUI.createCustomSlider("radius", { enabled: true, value: 0, min: 0, max: 5, step: 0.25, text: "Path Radius", slide: (event, newValue) => {
          maze.inputs["radius"] = newValue;
        },
      }),
      customUI.createCustomSlider("resolution", { enabled: true, value: 1, min: 1, max: 3, step: 1, text: "Mesh Resolution", slide: (event, newValue) => {
          maze.inputs["resolution"] = newValue;
        },
      }),
    ],
    [
      startTypeSelector,
    ],
    [
      customUI.createCustomSlider("centerX", { enabled: false, value: 1, min: 1, max: 20, step: 1, text: "Center Width", slide: (event, newValue) => {
          maze.inputs["centerX"] = newValue;
        },
      }),
      customUI.createCustomSlider("centerY", { enabled: false, value: 1, min: 1, max: 20, step: 1, text: "Center Height", slide: (event, newValue) => {
          maze.inputs["centerY"] = newValue;
        },
      }),
    ],
    [ 
      customUI.createCustomCheckbox("blocky", { value: false, text: "Blocky", change: (event, newValue) => {
          maze.inputs['blocky'] = newValue;
        },
      }),
    ],
  ];
  maze.addControls(controls);

  
  getNewMaze();

})();

var maze3 = (function() {

  function getNewMaze() {
    if (!maze.loading) {
      maze.loading = true;
      maze.fetchMaze().then((data) => {
        mazeFunctions.parseMeshMaze3D(data, maze);
        fillMazeBuffers();
        requestAnimationFrame(drawScene);
        maze.loading = false;
      })
    }
  };

  var maze = new Maze(document.querySelector("#third-maze-container"), '/maze_generator3.php', getNewMaze,
    {
        "width": 20 ,
        "height": 20,
        "depth": 3,
        "resolution": 1,
        "radius": 0,
        "startType": "edge",
        "centerX": 1,
        "centerY": 1,
    },
    'webgl',
  );
  var startTypeSelector = customUI.createCustomSelect("startType", {
    text: "Start Location",
    options: [
      { text: "Edge", value: "edge" }, { text: "Center", value: "center" },
    ],
    change: ((event, value) => {
      maze.inputs["startType"] = value;
      var centerSliderX = maze.container.querySelector(".slider-container[name='centerX']");
      var centerSliderY = maze.container.querySelector(".slider-container[name='centerY']");
      if (value == "edge") {
        centerSliderX.querySelector('input[type="range"]').disabled = true;
        centerSliderY.querySelector('input[type="range"]').disabled = true;
        centerSliderX.style.opacity = 0.25;
        centerSliderY.style.opacity = 0.25;
      } else {
        centerSliderX.querySelector('input[type="range"]').disabled = false;
        centerSliderY.querySelector('input[type="range"]').disabled = false;
        centerSliderX.style.opacity = 1;
        centerSliderY.style.opacity = 1;
      }
    }),
  });
  var controls = [
    [
      customUI.createCustomSlider("width", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Width", slide: (event, newValue) => {
          maze.inputs["width"] = newValue;
        },
      }),
      customUI.createCustomSlider("height", { enabled: true, value: 20, min: 4, max: 150, step: 1, text: "Height", slide: (event, newValue) => {
          maze.inputs["height"] = newValue;
        },
      }),
      customUI.createCustomSlider("depth", { enabled: true, value: 3, min: 1, max: 150, step: 1, text: "Depth", slide: (event, newValue) => {
        maze.inputs["depth"] = newValue;
      },
    }),
    ],
    [
      customUI.createCustomSlider("radius", { enabled: true, value: 0, min: 0, max: 5, step: 0.25, text: "Path Radius", slide: (event, newValue) => {
          maze.inputs["radius"] = newValue;
        },
      }),
      customUI.createCustomSlider("resolution", { enabled: true, value: 1, min: 1, max: 3, step: 1, text: "Mesh Resolution", slide: (event, newValue) => {
          maze.inputs["resolution"] = newValue;
        },
      }),
    ],
    [
      startTypeSelector,
    ],
    [
      customUI.createCustomSlider("centerX", { enabled: false, value: 1, min: 1, max: 20, step: 1, text: "Center Width", slide: (event, newValue) => {
          maze.inputs["centerX"] = newValue;
        },
      }),
      customUI.createCustomSlider("centerY", { enabled: false, value: 1, min: 1, max: 20, step: 1, text: "Center Height", slide: (event, newValue) => {
          maze.inputs["centerY"] = newValue;
        },
      }),
    ],
    [
      customUI.createCustomSlider("zoom", { enabled: true, value: 60, min: 1, max: 100, step: 1, text: "Zoom", slide: (event, newValue) => {
          maze.state.scale = newValue/100 * 10;
        },
      }),
    ],
  ];
  maze.addControls(controls);

  maze.state = {
    ui: {
      dragging: false,
      mouse: {
        lastX: -1,
        lastY: -1,
      },
    }, 
    angle: {
      x: degToRad(0),
      y: degToRad(0),
      z: degToRad(0),
    },
    scale: 6,
    translation: {
      x: 0,
      y: 0,
      z: -100,
    },
    fieldOfViewRadians: degToRad(60),
  }

  

  var canvas = maze.canvas;
  var gl = maze.gl;
  maze.glExt = gl.getExtension('OES_element_index_uint');
  if (!gl) {
    alert("Error: WebGL failed to load");
    return;
  }

  setupMeshMazeGlData();
  getNewMaze();

  // Click and drag to rotate. Scroll to scale
  function mouseDown(e) {
    var x = event.clientX;
    var y = event.clientY;
    var rect = event.target.getBoundingClientRect();
    // If in rectangle, mouse is within canvas
    if (rect.left <= x && x < rect.right && rect.top < y && y < rect.bottom) {
      maze.state.ui.mouse.lastX = x;
      maze.state.ui.mouse.lastY = y;
      maze.state.ui.dragging = true;
    }
  };

  function mouseUp(e){
    maze.state.ui.dragging = false;
  };

  function mouseMove(e) {
    var x = event.clientX;
    var y = event.clientY;
    if (maze.state.ui.dragging) {
      var factor = 5/canvas.height;
      var dx = factor * (x - maze.state.ui.mouse.lastX);
      var dy = factor * (y - maze.state.ui.mouse.lastY);
      // update angle
      maze.state.angle.x = maze.state.angle.x + dy;
      maze.state.angle.y = maze.state.angle.y + dx;
    }
    // update last mouse pos
    maze.state.ui.mouse.lastX = x;
    maze.state.ui.mouse.lastY = y;
  };

  function mouseScroll(e) {
    event.preventDefault();
    maze.state.scale += e.deltaY * -0.01;
    maze.state.scale = Math.min(Math.max(0.01, maze.state.scale), 4);
  };

  // Set callbacks for mouse movement
  canvas.onmousedown = mouseDown;
  canvas.onmouseup = mouseUp;
  canvas.onmousemove = mouseMove;
  canvas.onwheel = mouseScroll;

  // Fill maze buffers
  function fillMazeBuffers() {
    // Set universal translationsa
    maze.state.translation.z = Math.max(maze.size.x*maze.state.scale, maze.size.y*maze.state.scale)/Math.min(canvas.width, canvas.height) * -2.5;
    maze.state.scale = 1
    maze.state.angle.x = 0
    maze.state.angle.y = 0
    // Fill position buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, maze.glData.buffers.positionBuffer);
    var vertexArray = Float32Array.from(maze.objects.vertices);
    gl.bufferData(gl.ARRAY_BUFFER, vertexArray, gl.STATIC_DRAW);
    // Fill index buffer
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, maze.glData.buffers.indexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(maze.objects.indices), gl.STATIC_DRAW);
    // Fill normal buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, maze.glData.buffers.normalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(maze.objects.normals), gl.STATIC_DRAW);
  };
  
  // Setup necessary WebGL variables
  function setupMeshMazeGlData() {
    maze.glData = {
      vertexShader: undefined,
      fragmentShader: undefined,
      program: undefined,
      attributeLocations: {
        positionLocation: undefined,
        normalLocation: undefined,
      },
      uniformLocations: {
        worldViewProjectionLocation: undefined,
        colorLocation: undefined,
        reverseLightDirectionLocation: undefined,
        worldInverseTransposeLocation: undefined,
      },
      buffers: {
        positionBuffer: undefined,
        indexBuffer: undefined,
        normalBuffer: undefined,
      }
    };
    //setup GLSL program
    maze.glData.vertexShader = createShader(maze.gl, maze.gl.VERTEX_SHADER, vsSource);
    maze.glData.fragmentShader = createShader(maze.gl, maze.gl.FRAGMENT_SHADER, fsSource);
    
    // setup program
    maze.glData.program = createProgram(maze.gl, maze.glData.vertexShader, maze.glData.fragmentShader);

    // look up where the vertex data needs to go.
    maze.glData.attributeLocations.positionLocation = maze.gl.getAttribLocation(maze.glData.program, "a_position");
    maze.glData.attributeLocations.normalLocation = maze.gl.getAttribLocation(maze.glData.program, "a_normal");
    //maze.glData.colorLocation = maze.gl.getAttribLocation(maze.glData.program, "a_color");

    // lookup uniforms
    maze.glData.uniformLocations.worldViewProjectionLocation = maze.gl.getUniformLocation(maze.glData.program, "u_worldViewProjection");
    maze.glData.uniformLocations.colorLocation = maze.gl.getUniformLocation(maze.glData.program, "u_color");
    maze.glData.uniformLocations.reverseLightDirectionLocation = maze.gl.getUniformLocation(maze.glData.program, "u_reverseLightDirection");
    maze.glData.uniformLocations.worldInverseTransposeLocation = maze.gl.getUniformLocation(maze.glData.program, "u_worldInverseTranspose");

    // Create a buffer to put vertex positions in
    maze.glData.buffers.positionBuffer = maze.gl.createBuffer();
    // Create a buffer to put triangle indices in
    maze.glData.buffers.indexBuffer = maze.gl.createBuffer();
    // Create Buffer to put normals in
    maze.glData.buffers.normalBuffer = maze.gl.createBuffer();
  }

  // Update loop
  function drawScene() {
    
    mazeFunctions.drawMeshMaze3D(maze); // Draw

    requestAnimationFrame(drawScene);
  }
})();
