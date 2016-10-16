// Copyright 2016 Wayne D Grant (waynedgrant.com)
// Licensed under the MIT License

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
    
  var locationText = '"(' + pos.coords.latitude + ',' + pos.coords.longitude + ')"';
    
  var weatherLookupUrl =
    'https://query.yahooapis.com/v1/public/yql' +
    '?q=select location.city, item.condition.temp from weather.forecast where u="c" and woeid in (SELECT woeid FROM geo.places WHERE text=' + locationText + ')' +
    '&format=json';
  
  console.log('weatherLookupUrl=' + weatherLookupUrl);
  
  xhrRequest(weatherLookupUrl, 'GET', 
    function(responseJson) {
            
      console.log('responseJson=' + responseJson);
      
      var json = JSON.parse(responseJson);
      var location = json.query.results.channel.location.city;
      var temperature = parseInt(json.query.results.channel.item.condition.temp);
      
      console.log('location=' + location);
      console.log('temperature=' + temperature);
            
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
    
    var keys = require('message_keys');
    
    dictionary[keys.LOCATION] = location;
    dictionary[keys.TEMPERATURE] = temperature;
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
    {
      enableHighAccuracy: false,
      timeout: 15000,
      maximumAge: 60000
    }
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
