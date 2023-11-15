const char body[] PROGMEM = R"===( 
<!DOCTYPE html>
<html>
  <body>
    <h1>Motor Direction/Speed Sliders</h1>

    <div class="slidecontainer">
      <p>Direction Move Forward/Stop/Backward:</p>
      <input type="range" min="1" max="3" value="2" id="directionSlider">
      <span id="directionValue">Move Stop</span> <br>

      <p>Speed (DutyCycle) 0% - 100%:</p>
      <input type="range" min="0" max="100" value="50" id="dutyCycleSlider">
      <span id="dutyCycleValue">50 %</span> <br>

      <p>Servomotor (DutyCycle) 0% - 100%:</p>
      <input type="range" min="0" max="100" value="50" id="ServoSlider">
      <span id="ServoValue">50 %</span> <br>
    </div>
  </body>
  <script>
    var directionSlider = document.getElementById("directionSlider");
    var dutyCycleSlider = document.getElementById("dutyCycleSlider");
    var ServoSlider = document.getElementById("ServoSlider");

    directionSlider.oninput = function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("directionValue").innerHTML = this.responseText;
        }
      };
      var str = "/setDirection?val="; // Modify this URL to match your backend endpoint
      var res = str.concat(this.value);
      xhttp.open("GET", res, true);
      xhttp.send();
    }

    dutyCycleSlider.oninput = function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("dutyCycleValue").innerHTML = this.responseText + "%";
        }
      };
      var str = "/setDutyCycle?val="; // Modify this URL to match your backend endpoint
      var res = str.concat(this.value);
      xhttp.open("GET", res, true);
      xhttp.send();
    }
    
    ServoSlider.oninput = function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("ServoValue").innerHTML = this.responseText + "%";
        }
      };
      var str = "/setServo?val="; // Modify this URL to match your backend endpoint
      var res = str.concat(this.value);
      xhttp.open("GET", res, true);
      xhttp.send();
    }
  </script>
</html>

)===";
