'use strict';

var request = require('request');

exports.handler = function(event, context, callback){
  if (!event.authToken) {
    callback('Could not find authToken');
    return;
  }
  
  var options = {
    url: 'https://'+ process.env.DOMAIN + '/userinfo',
    method: 'GET',
    json: true,
    headers: {
      'Authorization': event.authToken
    }
  };

  request(options, function(error, response, body){
    if (!error && response.statusCode === 200) {
      console.log('Success to get user info.');
      callback(null, body);
    } else {
      callback(error);
    }
  });
};
