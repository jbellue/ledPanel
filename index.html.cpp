#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
    <head>
        <meta charset="utf-8">
        <title></title>
        <meta name="description" content="">
        <style>
body {
    background: #0e0e0e;
    color: #a2faa3;
    font-family: 'Ubuntu', sans-serif;
}
.grid-container {
  display: grid;
  grid-template-columns: 1fr 1fr;
  grid-template-rows: 0.7fr 1.3fr 1fr;
}
.range { grid-area: 1 / 2 / 2 / 3; }
.iro1 { grid-area: 2 / 2 / 3 / 3; }
.iro2 { grid-area: 3 / 2 / 4 / 3; }
.menu { grid-area: 1 / 1 / 4 / 2; }
.stv-radio-buttons-wrapper { display: inline-block; }
.stv-radio-button { display: none; }
.stv-radio-button + label {
    font-weight: bold;
    float: left;
    padding: 1em 1em;
    cursor: pointer;
    background: #1c1c1c;
    border: 2px solid #534b52;
    width: 9em;
    text-align: center;
}
.stv-radio-buttons-wrapper div:not(:last-child) label { border-bottom-style: none; }

.stv-radio-button:checked + label {
    color: #004966;
    background: #92c9b1;
}

        </style>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://cdn.jsdelivr.net/npm/@jaames/iro/dist/iro.min.js"></script>
    </head>
    <body>
        <div class="grid-container">
            <div class="range">
                <div class="">
                    <label for="brightness">Brightness</label>
                    <input class="" type="range" id="brightness" name="brightness" min="0" max="255" oninput="sendUpdate({brightness: this.value})">
                </div>
                <div class="">
                    <label for="speed">Speed</label>
                    <input class="" type="range" id="speed" name="speed" min="0" max="4" oninput="sendUpdate({speed: this.value})">
                </div>
            </div>
            <div class="iro1" id="color-picker-container1"></div>
            <div class="iro2" id="color-picker-container2"></div>
            <div class="menu">
                <div id="patternsGroup" class="stv-radio-buttons-wrapper">
                </div>
            </div>
        </div>
<script>
let connection; // websocket connection
let brightnessSlider;
let colorPicker1;
let colorPicker2;
let patternsObject;

const sendUpdate = data => {
    if (connection.readyState == 1) { // OPEN
        connection.send(JSON.stringify(data));
    }
}
const ready = fn => {
    if (document.readyState != 'loading'){
        fn();
    } else if (document.addEventListener) {
        document.addEventListener('DOMContentLoaded', fn);
    } else {
        document.attachEvent('onreadystatechange', () => {
            if (document.readyState != 'loading') {
                fn();
            }
        });
    }
};

const rgbToInt = rgb => {
    return (rgb.r << 16) | (rgb.g << 8) | (rgb.b);
}
const intToRgb = data => {
    return {r: (data & 0xff0000) >> 16, g: (data & 0x00ff00) >> 8, b: (data & 0x0000ff)};
}
const changePatternEvent = e => {
    showOrHideColourPickers(patternsObject[e.target.value].numColour);
    sendUpdate({pattern: e.target.value});
}
const updatePage = settings => {
    if(settings.brightness) {
        brightnessSlider.value = settings.brightness;
    }
    if(settings.colour1) {
        colorPicker1.color.set(intToRgb(settings.colour1));
    }
    if(settings.colour2) {
        colorPicker2.color.set(intToRgb(settings.colour2));
    }
};

const showOrHideColourPickers = num => {
    if (num == 0) {
        document.getElementById('color-picker-container1').style.display = "none";
        document.getElementById('color-picker-container2').style.display = "none";
    }
    else if (num == 1) {
        document.getElementById('color-picker-container1').style.display = "";
        document.getElementById('color-picker-container2').style.display = "none";
    }
    else {
        document.getElementById('color-picker-container1').style.display = "";
        document.getElementById('color-picker-container2').style.display = "";
    }
};

const setPatterns = (patterns, selectedPattern) => {
    patternsObject = patterns;
    let parent = document.getElementById("patternsGroup");
    const arraySize = patterns.length;
    for (let i = 0; i < arraySize; ++i) {
        let radioDiv = document.createElement('div');
        let radioInput = document.createElement('input');
        radioInput.classList.add("stv-radio-button");
        radioInput.type = "radio";
        const radioId = patterns[i].name.replace(/\s/g,'');
        radioInput.id = patterns[i].name.replace(/\s/g,'');
        radioInput.name = "radioPattern";
        radioInput.value = i;
        if (i == selectedPattern) {
            radioInput.checked = true;
            showOrHideColourPickers(patterns[i].numColour);
        }
        radioInput.onchange = changePatternEvent;
        let radioLabel = document.createElement('label');
        radioLabel.htmlFor = radioId;
        radioLabel.innerText = patterns[i].name;
        radioDiv.appendChild(radioInput);
        radioDiv.appendChild(radioLabel);
        parent.appendChild(radioDiv);
    }
};

ready(() => {
    colorPicker1 = new iro.ColorPicker('#color-picker-container1', {width:150, display: "inline-block"});
    colorPicker2 = new iro.ColorPicker('#color-picker-container2', {width:150, display: "inline-block"});
    brightnessSlider = document.getElementById("brightness")
    connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
    connection.onerror = error => {
        console.log('WebSocket Error ', error);
    };
    connection.onmessage = (e) => {
        console.log(e.data);
        jsonData = JSON.parse(e.data);
        if(jsonData) {
            let selectedPattern = -1;
            if (jsonData.settings) {
                updatePage(jsonData.settings);
                selectedPattern = jsonData.settings.pattern;
            }
            if (jsonData.patterns) {
                setPatterns(jsonData.patterns, selectedPattern);
            }
        }
    };
    colorPicker1.on('color:change', (color, event) => {
        sendUpdate({colour1: rgbToInt(color.rgb)});
    });
    colorPicker2.on('color:change', (color, event) => {
        sendUpdate({colour2: rgbToInt(color.rgb)});
    });
});

</script>
    </body>
</html>
)=====";
    
