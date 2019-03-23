#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
    <head>
        <meta charset="utf-8">
        <title></title>
        <meta name="description" content="">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/semantic-ui/2.4.1/semantic.min.css" />
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://cdn.jsdelivr.net/npm/@jaames/iro/dist/iro.min.js"></script>
    </head>
    <body>
        <div class="ui center aligned container">
            <div class="ui form">
                <div class="field">
                    <label for="brightness">Brightness</label>
                    <input class="ui input" type="range" id="brightness" name="brightness" min="0" max="255" oninput="sendUpdate({brightness: this.value})">
                </div>
                <div class="field">
                    <label for="speed">Speed</label>
                    <input class="ui input" type="range" id="speed" name="speed" min="0" max="4" oninput="sendUpdate({speed: this.value})">
                </div>
            </div>
            <div class="ui vertical buttons" id="patternsGroup">
            </div>
            <div id="colorPickers" class="ui grid ceter aligned">
                <div class="eight wide column" id="color-picker-container1"></div>
                <div class="eight wide column" id="color-picker-container2"></div>
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
    document.querySelectorAll("#patternsGroup .active").forEach(btn => {
        btn.classList.remove("active");
    });
    e.target.classList.add("active");
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
        let button = document.createElement('button');
        button.onclick = changePatternEvent;
        button.value = i;
        button.innerHTML = patterns[i].name;
        if (i == selectedPattern) {
            button.classList.add("active");
            showOrHideColourPickers(patterns[i].numColour)
        }
        button.classList.add("ui", "button");
        parent.appendChild(button);
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
    
