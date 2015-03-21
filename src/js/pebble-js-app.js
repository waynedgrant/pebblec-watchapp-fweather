var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  lookupWeather(pos);
}

function lookupWeather(pos) {
  var url =
      'https://query.yahooapis.com/v1/public/yql?q=select city, woeid from geo.placefinder where text="' +
      pos.coords.latitude + ',' +
      pos.coords.longitude +
      '" and gflags="R"&format=json';
  
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      var location = json.query.results.Result.city;
      var woeid = json.query.results.Result.woeid;
            
      lookupTemperatureByWoeid(location, woeid);
    }
  );
}

function lookupTemperatureByWoeid(location, woeid) {
  var url =
      'https://query.yahooapis.com/v1/public/yql?q=select item.condition.temp from weather.forecast where woeid=' +
      woeid + ' and u="c"&format=json';
    
  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);

      var temperature = parseInt(json.query.results.channel.item.condition.temp);
                
      sendToPebble(location, temperature);
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location: ' + err);
  sendToPebble(null, null);
}

function sendToPebble(location , temperature){
  var dictionary = {};
  
  if (location !== null && temperature !== null) {
    dictionary = {
      'KEY_LOCATION' : location,
      'KEY_TEMPERATURE': temperature
    };
  }
      
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log('Weather info sent to Pebble successfully');
    },
    function(e) {
      console.log('Error sending weather info to Pebble!');
    }
  );
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready', 
  function(e) {
    getWeather();
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    getWeather();
  }                     
);
