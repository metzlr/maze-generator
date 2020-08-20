var canvasFunctions = (function() {
  function resizeCanvas(canvas) {
    var realToCSSPixels = window.devicePixelRatio;
    // Lookup the size the browser is displaying the canvas in CSS pixels
    // and compute a size needed to make our drawingbuffer match it in
    // device pixels. Can significantly lower performance.
    var displayWidth  = Math.floor(canvas.clientWidth  * realToCSSPixels);
    var displayHeight = Math.floor(canvas.clientHeight * realToCSSPixels);
    // Check if the canvas is not the same size.
    //console.log(displayWidth, displayHeight, canvas.width, canvas.height, canvas.clientWidth, canvas.clientHeight, realToCSSPixels);
    if (canvas.width  !== displayWidth ||
      canvas.height !== displayHeight) {
      // Make the canvas the same size
      canvas.width  = displayWidth;
      canvas.height = displayHeight;
    }
  }

  function createCanvasCheckbox(name, options) {
    var element = document.createElement("label");
    element.className = "flex-row-centered";
    element.innerHTML = `${options.text}
      <input type="checkbox" class = "canvas-checkbox" name="${name}" value="${options.value}" autocomplete="off">
    `;
    element.addEventListener('change', (event) => {
      var newValue;
      if (event.target.value == "false") {
        event.target.value = "true";
        newValue = true;
      } else {
        event.target.value = "false";
        newValue = false;
      }
      options.change(event, newValue);
    });
    return element;
  }

  return {
    resizeCanvas: resizeCanvas,
    createCanvasCheckbox: createCanvasCheckbox,
  }
})();