#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
    <head>
        <meta charset="utf-8">
        <title>LED Panel</title>
        <meta name="description" content="">
        <style>
body {
    color: #fff;
    background: #171F30;
    font-family: 'Ubuntu', sans-serif;
    text-align: center;
}
.wrap { 
    min-height: 75vh;
    max-width: 720px;
    margin: 0 auto;
    display: flex;
    flex-direction: row;
}
.wrap .half {
    width: 50%;
}
.range label {display: block;}
.range input {width: 100%;}
.radioButton { display: none; }
.radioButton + label {
    float: left;
    padding: 1em;
    cursor: pointer;
    background: #444857;
    margin-bottom: 7px;
    border-radius: 4px;
    min-width: 150px;
}
.radioButton:hover + label {
    background: #5a5f73;
}
.radioButton:checked + label {
    color: #000;
    background: #47cf73;
}
        </style>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://cdn.jsdelivr.net/npm/@jaames/iro@4.3.2/dist/iro.min.js"></script>
    </head>
    <body>
        <h1>LED Panel</h1>
        <div class="wrap">
            <div class="menu half">
                <div id="patternsGroup">
                </div>
            </div>
            <div class="half">
                <form class="range" autocomplete="off">
                    <div>
                        <label for="brightness">Brightness</label>
                        <input class="" type="range" id="brightness" name="brightness" min="0" max="255" oninput="sendUpdate({brightness: this.value})">
                    </div>
                    <div>
                        <label for="speed">Speed</label>
                        <input class="" type="range" id="speed" name="speed" min="0" max="4" oninput="sendUpdate({speed: this.value})">
                    </div>
                </form>
                <div class="iro1" id="color-picker-container1"></div>
                <div class="iro2" id="color-picker-container2"></div>
            </div>
        </div>
<script>
let connection; // websocket connection
let brightnessSlider;
let speedSlider;
let colorPicker1;
let colorPicker2;
let patternsObject;
let websocketId;

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
        radioInput.classList.add("radioButton");
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
    colorPicker1 = new iro.ColorPicker('#color-picker-container1', {width:200, display: "inline-block", borderWidth: 1, borderColor: "#fff"});
    colorPicker2 = new iro.ColorPicker('#color-picker-container2', {width:200, display: "inline-block", borderWidth: 1, borderColor: "#fff"});
    brightnessSlider = document.getElementById("brightness");
    speedSlider = document.getElementById("speed");
    connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
    connection.onerror = error => {
        console.log('WebSocket Error ', error);
    };
    connection.onmessage = rawData => {
        console.log(rawData.data);
        jsonData = JSON.parse(rawData.data);
        if(jsonData) {
            if ("id" in jsonData) {
                // this should be on connection, so always set the data
                websocketId = jsonData.id;
                let selectedPattern = -1;
                if (jsonData.settings) {
                    updatePage(jsonData.settings);
                    selectedPattern = jsonData.settings.pattern;
                }
                if (jsonData.patterns) {
                    setPatterns(jsonData.patterns, selectedPattern);
                }
            }
            else if (jsonData.settings && ("sender" in jsonData) && jsonData.sender != websocketId) {
                updatePage(jsonData.settings);
            }
        }
    };
    colorPicker1.on("input:change", color => {
        sendUpdate({colour1: rgbToInt(color.rgb)});
    });
    colorPicker2.on("input:change", color => {
        sendUpdate({colour2: rgbToInt(color.rgb)});
    });
});

</script>
    </body>
</html>
)=====";
    
