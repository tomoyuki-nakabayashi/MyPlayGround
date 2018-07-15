var userController = {
  data: {
    auth0Lock: null,
    config: null
  },
  uiElements: {
    loginButton: null,
    logoutButton: null,
    profileButton: null,
    profileNameLabel: null,
    profileImage: null
  },
  init: function(config) {
    var that = this;

    this.uiElements.loginButton = $('#auth0-login');
    this.uiElements.logoutButton = $('#auth0-logout');
    this.uiElements.profileButton = $('#user-profile');
    this.uiElements.profileNameLabel = $('#profilename');
    this.uiElements.profileImage = $('#profilepicture');

    this.data.config = config;
    this.data.auth0Lock = new Auth0Lock(config.auth0.clientId, config.auth0.domain, {
      auth: {
        redirectUrl: 'http://127.0.0.1:8000',
        responseType: 'code',
        params: {
          scope: 'openid email user_metadata picture' // Learn about scopes: https://auth0.com/docs/scopes
        }
      }
    });

    var idToken = localStorage.getItem('accessToken');

    if (idToken) {
      this.configureAuthenticatedRequests();
      this.data.auth0Lock.getUserInfo(idToken, function(err, profile) {
        if (err) {
          return alert('There was an error getting the profile: ' + err.message);
        }
        that.showUserAuthenticationDetails(profile);
      });
    }

    this.wireEvents();
  },
  configureAuthenticatedRequests: function() {
    $.ajaxSetup({
      'beforeSend': function(xhr) {
        xhr.setRequestHeader('Authorization', 'Bearer ' + localStorage.getItem('accessToken'));
      }
    });
  },
  showUserAuthenticationDetails: function(profile) {
    var showAuthenticationElements = !!profile;

    if (showAuthenticationElements) {
      this.uiElements.profileNameLabel.text(profile.nickname);
      this.uiElements.profileImage.attr('src', profile.picture);
    }

    this.uiElements.loginButton.toggle(!showAuthenticationElements);
    this.uiElements.logoutButton.toggle(showAuthenticationElements);
    this.uiElements.profileButton.toggle(showAuthenticationElements);
  },
  wireEvents: function() {
    var that = this;

    this.data.auth0Lock.on("authenticated", function(authResult) {
      that.data.auth0Lock.getUserInfo(authResult.accessToken, function(error, profile) {
        that.data.auth0Lock.hide();
        if (error) return alert("Auth0 error:" + error);

        localStorage.setItem("idToken", authResult.idToken);
        localStorage.setItem("accessToken", authResult.accessToken);
        that.configureAuthenticatedRequests();
        that.showUserAuthenticationDetails(profile);
      })
    });

    this.uiElements.loginButton.click(function(e) {
      that.data.auth0Lock.show({
        auth: {
          params: {
              responseType: 'id_token token'
          }
        }
      });
    });

    this.uiElements.logoutButton.click(function(e) {
      localStorage.removeItem('userToken');

      that.uiElements.logoutButton.hide();
      that.uiElements.profileButton.hide();
      that.uiElements.loginButton.show();
    });

    this.uiElements.profileButton.click(function (e) {
      var url = that.data.config.apiBaseUrl + '/user-profile';
      $.get(url, function(data, status) {
        $('#user-profile-raw-json').text(JSON.stringify(data, null, 2));
        $('#user-profile-modal').modal();
      })
    });
  }
}