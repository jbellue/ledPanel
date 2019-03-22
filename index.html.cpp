#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
    <head>
        <meta charset="utf-8">
        <title></title>
        <meta name="description" content="">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://cdn.jsdelivr.net/npm/@jaames/iro/dist/iro.min.js"></script>
    </head>
    <body>
        <div>
            <div>
                <label for="brightness">Brightness</label>
                <div>
                    <input type="range" id="brightness" name="brightness" min="0" max="255" oninput="sendUpdate({brightness: this.value})">
                </div>
            </div>
            <div>
                <label for="speed">Speed</label>
                <div>
                    <input type="range" id="speed" name="speed" min="0" max="4" oninput="sendUpdate({speed: this.value})">
                </div>
            </div>
        </div>
        <div id="patternsGroup"></div>
        <div id="color-picker-container1"></div>
        <div id="color-picker-container2"></div>
<script>
let connection; // websocket connection
let brightnessSlider;
let colorPicker1;
let colorPicker2;

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
const setPatterns = (patterns, selectedPattern) => {
    let parent = document.getElementById("patternsGroup");
    const arraySize = patterns.length;
    for (let i = 0; i < arraySize; ++i) {
        let radioDiv = document.createElement('div');
        let radioInput = document.createElement('input');
        radioInput.type = "radio";
        radioInput.id = patterns[i].replace(/\s/g,'');
        radioInput.name = "radioPattern";
        radioInput.value = i;
        if (i == selectedPattern) {
            radioInput.checked = true;
        }
        radioInput.onchange = changePatternEvent;
        let radioLabel = document.createElement('label');
        radioLabel.htmlFor = patterns[i].replace(/\s/g,'');
        radioLabel.innerText = patterns[i];
        radioDiv.appendChild(radioInput);
        radioDiv.appendChild(radioLabel);
        parent.appendChild(radioDiv);
    }
};

ready(() => {
    colorPicker1 = new iro.ColorPicker('#color-picker-container1');
    colorPicker2 = new iro.ColorPicker('#color-picker-container2');
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
    
