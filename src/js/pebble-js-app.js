var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function lookup_woeid(pos) {
  console.log('Success requesting location!');

  var url =
      'https://query.yahooapis.com/v1/public/yql?q=select city, woeid from geo.placefinder where text="' +
      pos.coords.latitude + ',' +
      pos.coords.longitude +
      '" and gflags="R"&format=json';

  console.log(url);
  
  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      var location = json.query.results.Result.city;
      var woeid = json.query.results.Result.woeid;
      
      console.log("location: " + location + ", woeid: " + woeid);
      
      lookup_temp(location, woeid);
    }
  );
}

function lookup_temp(location, woeid) {
  
  var url =
      'https://query.yahooapis.com/v1/public/yql?q=select item.condition.temp from weather.forecast where woeid=' +
      woeid + ' and u="c"&format=json';
  
  console.log(url);
  
  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);

      var temperature = parseInt(json.query.results.channel.item.condition.temp);
          
      console.log("temperature: " + temperature);
      
      var dictionary = {
        'KEY_LOCATION' : location,
        'KEY_TEMPERATURE': temperature
      };
      
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      );
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    lookup_woeid,
    locationError, // TODO - proper failure handler
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
