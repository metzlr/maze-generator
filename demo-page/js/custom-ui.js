var customUI = (function() {
  function createCustomSlider(name, options) {
    var float = options.float || false;
    var min = options.min || 0;
    var max = options.max || 1;
    var step = options.step || 1;
    var value = options.value || 0;
    var updateFunction = options.slide;
    var labelText = options.text || "Label";
    
    var sliderDiv = document.createElement("div");
    sliderDiv.className = "slider-container flex-col-centered";
    sliderDiv.setAttribute("name", name);
    sliderDiv.innerHTML = `
      <p class="slider-value">${labelText}: <span>${value}</span></p>
      <input type="range" name="${name}" min="${min}" max="${max}" step="${step}" value="${value}" class="slider" autocomplete="off">
    `;
    var valueElement = sliderDiv.querySelector(".slider-value").querySelector("span");
    var sliderElement = sliderDiv.querySelector(".slider");

    function changeHandler(event) {
      var value = parseFloat(event.target.value);
      valueElement.innerHTML = value;
      updateFunction(event, value);
    }
    sliderElement.addEventListener('input', changeHandler);
    sliderElement.addEventListener('change', changeHandler);
    if (!options.enabled) {
      sliderElement.disabled = true;
      sliderDiv.style.opacity = 0.25;
    }
    return sliderDiv;
  }

  function createCustomCheckbox(name, options) {
    var value = options.value || false
    var text = options.text || "Label";
    var element = document.createElement("label");
    element.className = "check-container";
    element.setAttribute("name", name);
    element.innerHTML = `${text}
      <input type="checkbox" class = "checkbox" name="${name}" value="${value}" autocomplete="off">
      <span class="checkmark"></span>
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
    //parent.appendChild(element);
    return element;
  }

  function createCustomSelect(name, options) {
    var text = options.text || "Label";
    var element = document.createElement("div");
    element.className = "select-container";
    element.setAttribute("name", name);
    element.innerHTML = `
      <label class="select-label" for="select">${text}:</label>
      <select class="select" name="${name}"></select>
    `;
    options.options.forEach((item) => {
      element.querySelector("select").innerHTML += `
        <option value="${item.value}">${item.text}</option>
      `;
    });
    var select = element.querySelector("select");
    select.addEventListener('change', (event) => {
      newValue = select.options[select.selectedIndex].value;
      options.change(event, newValue);
    });
    return element;
  }

  return {
    createCustomSlider: createCustomSlider,
    createCustomCheckbox: createCustomCheckbox,
    createCustomSlider, createCustomSelect,
  }
})();