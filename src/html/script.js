let connection; // websocket connection
let brightnessSlider;
let speedSlider;
let colorPicker;
let patternsObject;
let websocketId;

const sendUpdate = (data) => {
    if (connection.readyState === 1) { // OPEN
        connection.send(JSON.stringify(data));
    }
};

const ready = (fn) => {
    if (document.readyState !== "loading"){
        fn();
    } else if (document.addEventListener) {
        document.addEventListener("DOMContentLoaded", fn);
    } else {
        document.attachEvent("onreadystatechange", () => {
            if (document.readyState !== "loading") {
                fn();
            }
        });
    }
};

const rgbToInt = (rgb) => {
    return (rgb.r << 16) | (rgb.g << 8) | (rgb.b);
};

const intToRgb = (data) => {
    return {r: (data & 0xff0000) >> 16,
            g: (data & 0x00ff00) >> 8,
            b: (data & 0x0000ff)};
};

const showOrHideColourPickers = (num) => {
    if (num === 0) {
        document.getElementById("color-picker-container").style.display = "none";
    }
    else {
        document.getElementById("color-picker-container").style.display = "";
        if (num === 1) {
            if (colorPicker.colors.length > 1) {
                colorPicker.removeColor(1);
            }
        }
        else {
            if (colorPicker.colors.length === 1) {
                colorPicker.addColor("rgb(100%, 100%, 100%)");
            }
        }
    }
};

const changePatternEvent = (e) => {
    showOrHideColourPickers(patternsObject[e.target.value].numColour);
    sendUpdate({pattern: e.target.value});
};

const updatePage = (settings) => {
    if("brightness" in settings) {
        brightnessSlider.value = settings.brightness;
    }
    if("speed" in settings) {
        speedSlider.value = settings.speed;
    }
    if("pattern" in settings) {
        // the radio buttons might not already be there at startup, so we check first
        let radioToSelect = document.querySelectorAll("input[name=radioPattern]")[settings.pattern]; 
        if (radioToSelect) {
            showOrHideColourPickers(patternsObject[settings.pattern].numColour);
            radioToSelect.checked = true;
        }
    }
    if(settings.colour1) {
        colorPicker.colors[0].rgb = intToRgb(settings.colour1);
    }
    if(settings.colour2) {
        if (colorPicker.colors.length === 2) {
            colorPicker.colors[1].rgb = intToRgb(settings.colour2);
        }
        else {
            if (patternsObject[settings.pattern].numColour === 2) {
                colorPicker.addColor(intToRgb(settings.colour2));
            }
        }
    }
};

const setPatterns = (patterns) => {
    patternsObject = patterns;
    let parent = document.getElementById("patternsGroup");
    const arraySize = patterns.length;
    for (let i = 0; i < arraySize; ++i) {
        let radioDiv = document.createElement("div");
        let radioInput = document.createElement("input");
        radioInput.classList.add("radioButton");
        radioInput.type = "radio";
        const radioId = patterns[i].name.replace(/\s/g,"");
        radioInput.id = radioId;
        radioInput.name = "radioPattern";
        radioInput.value = i;
        radioInput.onchange = changePatternEvent;
        let radioLabel = document.createElement("label");
        radioLabel.htmlFor = radioId;
        radioLabel.innerText = patterns[i].name;
        radioDiv.appendChild(radioInput);
        radioDiv.appendChild(radioLabel);
        parent.appendChild(radioDiv);
    }
};

const setSelectedPattern = (selectedPattern) => {
    document.querySelector(`input[value="${selectedPattern}"]`).checked = true;
};

const getReady = () => {
    colorPicker = new window.iro.ColorPicker("#color-picker-container", {width:200, display: "inline-block", borderWidth: 2, borderColor: "#fff"});
    brightnessSlider = document.getElementById("brightness");
    speedSlider = document.getElementById("speed");
    connection = new WebSocket(`ws://${location.hostname}/ws`, ["arduino"]);

    connection.onerror = (error) => {
        console.log("WebSocket Error ", error);
    };
    connection.onmessage = (rawData) => {
        console.log(rawData.data);
        const jsonData = JSON.parse(rawData.data);
        if(jsonData) {
            if ("id" in jsonData) {
                // this should be on connection, so always set the data
                websocketId = jsonData.id;
                if (jsonData.patterns) {
                    setPatterns(jsonData.patterns);
                }
                if (jsonData.settings) {
                    updatePage(jsonData.settings);
                    setSelectedPattern(jsonData.settings.pattern);
                }
            }
            else if (jsonData.settings && ("sender" in jsonData) && jsonData.sender !== websocketId) {
                updatePage(jsonData.settings);
            }
        }
    };
    colorPicker.on(["input:move", "input:end"], (color) => {
        let data = {};
        data[`colour${color.index + 1}`] = rgbToInt(color.rgb);
        sendUpdate(data);
    });
};
