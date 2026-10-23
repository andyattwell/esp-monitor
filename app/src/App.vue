<template>
<div id="app">
    <div id="layout">
        <!-- Menu toggle -->
        <a href="#menu" id="menuLink" class="menu-link" @click="toggleAll">
            <!-- Hamburger icon -->
            <span></span>
        </a>

        <div id="menu">
            <div class="pure-menu">
                <a class="pure-menu-heading" href="#">ESPMonitor</a>

                <ul class="pure-menu-list">
                    <li class="pure-menu-item"><router-link to="/" class="pure-menu-link">Inicio</router-link></li>
                    <li class="pure-menu-item"><router-link to="/wifi" class="pure-menu-link">Wifi</router-link></li>
                    <li class="pure-menu-item"><router-link to="/gsm" class="pure-menu-link">Gsm</router-link></li>
                </ul>
            </div>
        </div>

        <div id="main">
            <router-view v-if="loaded"/>
            <div class="loader-container main-loader" v-if="loading">
                <div class="loader"></div>
                <div class="loader-txt" v-html="status"></div>
            </div>
        </div>
    </div>
</div>

</template>

<script>
export default {
  name: 'App',
  data () {
    return {
      loading: false,
      loaded: false,
      navClass: '',
      status: 'Conectando al dispositivo...'
    }
  },
  mounted () {
    let self = this
    self.loading = true
    var content = document.getElementById('main')
    var menu = document.getElementById('menu')
    content.onclick = function (e) {
      if (menu.className.indexOf('active') !== -1) {
        self.toggleAll(e)
      }
    }
    this.testApi()
  },
  methods: {
    testApi () {
      let self = this
      console.log('Probando endpoint ' + this.$store.getters.endpoint)
      this.$http.get(self.$store.getters.endpoint + 'config')
        .then(function (response) {
          self.loading = false
          self.loaded = true
          this.$store.commit('config', response.data)
        }, (error) => {
          if (error) { }
          this.$store.commit('endpoint', 'http://192.168.1.100/')
          console.log('Probando endpoint ' + self.$store.getters.endpoint)
          console.log(error)

          self.$http.get(self.$store.getters.endpoint + 'config')
            .then(function (response) {
              this.$store.commit('config', response.data)
              self.loading = false
              self.loaded = true
            }, (error) => {
              if (error) {
                console.warn('Error conectando al dispositivo')
                console.log(error)
                self.status = 'Error conectando. <br> Por favor, compruebe la conexion Wifi'
              }
            })
        })
    },
    getConfig () {
      this.$http.get('config').then(response => {
        this.$store.config = response.data
        self.loading = false
        self.loaded = true
      }, response => {
        console.log(response)
        self.status = 'Error conectando. <br> Por favor, compruebe la conexion Wifi'
      })
    },
    toggleAll (e) {
      var active = 'active'
      var layout = document.getElementById('layout')
      var menu = document.getElementById('menu')
      var menuLink = document.getElementById('menuLink')
      e.preventDefault()
      this.toggleClass(layout, active)
      this.toggleClass(menu, active)
      this.toggleClass(menuLink, active)
    },
    toggleClass (element, className) {
      var classes = element.className.split(/\s+/)
      var length = classes.length
      var i = 0
      for (; i < length; i++) {
        if (classes[i] === className) {
          classes.splice(i, 1)
          break
        }
      }
      // The className is not found
      if (length === classes.length) {
        classes.push(className)
      }
      element.className = classes.join(' ')
    }
  }
}
</script>
<style>
/*
 * -- BASE STYLES --
 * Most of these are inherited from Base, but I want to change a few.
 */

body, html{
  width: 100%;
  height: 100%;
}

body {
    color: #333;
}

a {
    text-decoration: none;
    color: #1b98f8;
}

.pure-button-primary{
    background-color: #42a4ff;
    margin: 0 3px 0 0;
    color:#fff;
}

.button-success,
.button-error,
.button-warning,
.button-secondary {
    color: white;
    border-radius: 4px;
    text-shadow: 0 1px 1px rgba(0, 0, 0, 0.2);
}

.button-success {
    background: rgb(28, 184, 65); /* this is a green */
}

.button-error {
    background: rgb(202, 60, 60); /* this is a maroon */
}

.button-warning {
    background: rgb(223, 117, 20); /* this is an orange */
}

.button-secondary {
    background: rgb(66, 184, 221); /* this is a light blue */
}

.button-xsmall {
    font-size: 70%;
}

.pure-form{
    position: relative;
}

.main-loader{
    position: fixed !important;
}

.loader-container{
    display: block;
    content: "";
    position: absolute;
    width: 100%;
    height: 100%;
    background: rgba(255,255,255,0.6);
    top:0;
    left:0;
    z-index: 10000;
}

.loader {
    border: 5px solid #f3f3f3; /* Light grey */
    border-top: 5px solid #3498db; /* Blue */
    border-radius: 50%;
    width: 12px;
    height: 12px;
    animation: spin 1s linear infinite;
    z-index: 10001;
    position: absolute;
    left:50%;
    top:50%;
    margin-top:-15px;
    margin-left: -15px;
}

.loader-txt{
    letter-spacing: normal;
    position: absolute;
    width: 100%;
    padding-top: 15px;
    top:50%;
    left:0;
    text-align: center
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}

/* Email Content Styles */
.content-header, .content-body, .content-footer {
    padding: 1em 2em;
}
.content-body{
    position: relative;
}
.content-header {
  border-bottom: 1px solid #ddd;
}

.content-title {
  margin: 0.5em 0 0;
}
.content-subtitle {
  font-size: 1em;
  margin: 0;
  font-weight: normal;
}
.content-controls {
  margin-top: 2em;
  text-align: right;
}

Add transition to containers so they can push in and out.
*/
#layout,
#menu,
.menu-link {
    -webkit-transition: all 0.2s ease-out;
    -moz-transition: all 0.2s ease-out;
    -ms-transition: all 0.2s ease-out;
    -o-transition: all 0.2s ease-out;
    transition: all 0.2s ease-out;
}

/*
This is the parent `<div>` that contains the menu and the content area.
*/
#layout {
    position: relative;
    left: 0;
    padding-left: 0;
}
    #layout.active #menu {
        left: 150px;
        width: 150px;
    }

    #layout.active .menu-link {
        left: 150px;
    }
/*
The content `<div>` is where all your content goes.
*/
.content {
    margin: 0 auto;
    padding: 0 2em;
    max-width: 800px;
    margin-bottom: 50px;
    line-height: 1.6em;
}

.header {
     margin: 0;
     color: #333;
     text-align: center;
     padding: 2.5em 2em 0;
     border-bottom: 1px solid #eee;
 }
    .header h1 {
        margin: 0.2em 0;
        font-size: 3em;
        font-weight: 300;
    }
     .header h2 {
        font-weight: 300;
        color: #ccc;
        padding: 0;
        margin-top: 0;
    }

.content-subhead {
    margin: 50px 0 20px 0;
    font-weight: 300;
    color: #888;
}

/*
The `#menu` `<div>` is the parent `<div>` that contains the `.pure-menu` that
appears on the left side of the page.
*/

#menu {
    margin-left: -150px; /* "#menu" width */
    width: 150px;
    position: fixed;
    top: 0;
    left: 0;
    bottom: 0;
    z-index: 1000; /* so the menu or its navicon stays above all content */
    background: #191818;
    overflow-y: auto;
    -webkit-overflow-scrolling: touch;
}
    /*
    All anchors inside the menu should be styled like this.
    */
    #menu a {
        color: #999;
        border: none;
        padding: 0.6em 0 0.6em 0.6em;
    }

    /*
    Remove all background/borders, since we are applying them to #menu.
    */
     #menu .pure-menu,
     #menu .pure-menu ul {
        border: none;
        background: transparent;
    }

    /*
    Add that light border to separate items into groups.
    */
    #menu .pure-menu ul,
    #menu .pure-menu .menu-item-divided {
        border-top: 1px solid #333;
    }
        /*
        Change color of the anchor links on hover/focus.
        */
        #menu .pure-menu li a:hover,
        #menu .pure-menu li a:focus {
            background: #333;
        }

    /*
    This styles the selected menu item `<li>`.
    */
    #menu .pure-menu-selected,
    #menu .pure-menu-heading {
        background: #1f8dd6;
    }
        /*
        This styles a link within a selected menu item `<li>`.
        */
        #menu .pure-menu-selected a {
            color: #fff;
        }

    /*
    This styles the menu heading.
    */
    #menu .pure-menu-heading {
        font-size: 110%;
        color: #fff;
        margin: 0;
    }

/* -- Dynamic Button For Responsive Menu -------------------------------------*/

/*
The button to open/close the Menu is custom-made and not part of Pure. Here's
how it works:
*/

/*
`.menu-link` represents the responsive menu toggle that shows/hides on
small screens.
*/
.menu-link {
    position: fixed;
    display: block; /* show this only on small screens */
    top: 0;
    left: 0; /* "#menu width" */
    background: #000;
    background: rgba(0,0,0,0.7);
    font-size: 10px; /* change this value to increase/decrease button size */
    z-index: 10;
    width: 2em;
    height: auto;
    padding: 2.1em 1.6em;
}

    .menu-link:hover,
    .menu-link:focus {
        background: #000;
    }

    .menu-link span {
        position: relative;
        display: block;
    }

    .menu-link span,
    .menu-link span:before,
    .menu-link span:after {
        background-color: #fff;
        width: 100%;
        height: 0.2em;
    }

        .menu-link span:before,
        .menu-link span:after {
            position: absolute;
            margin-top: -0.6em;
            content: " ";
        }

        .menu-link span:after {
            margin-top: 0.6em;
        }

/* -- Responsive Styles (Media Queries) ------------------------------------- */

/*
Hides the menu at `48em`, but modify this based on your app's needs.
*/
@media (min-width: 48em) {

    .header,
    .content {
        padding-left: 2em;
        padding-right: 2em;
    }

    #layout {
        padding-left: 150px; /* left col width "#menu" */
        left: 0;
    }
    #menu {
        left: 150px;
    }

    .menu-link {
        position: fixed;
        left: 150px;
        display: none;
    }

    #layout.active .menu-link {
        left: 150px;
    }
}

@media (max-width: 48em) {
    /* Only apply this when the window is small. Otherwise, the following
    case results in extra padding on the left:
        * Make the window small.
        * Tap the menu to trigger the active state.
        * Make the window large again.
    */
    #layout.active {
        position: relative;
        left: 150px;
    }
}
</style>
