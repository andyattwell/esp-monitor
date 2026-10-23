import Vue from 'vue'
import App from './App'
import router from './router'
import VueResource from 'vue-resource'
import Vuex from 'vuex'

Vue.use(VueResource)
Vue.use(Vuex)

Vue.config.productionTip = false
// Vue.http.options.root = 'http://192.168.1.7/'
/* eslint-disable no-new */

const store = new Vuex.Store({
  state: {
    endpoint: 'http://192.168.4.1/',
    config: {}
  },
  getters: {
    endpoint: state => {
      return state.endpoint
    },
    config: state => {
      return state.config
    }
  },
  mutations: {
    // showing passed with payload, represented as num
    endpoint: (state, endpoint) => {
      state.endpoint = endpoint
    },
    config: (state, config) => {
      console.log('Mutando')
      console.log(state)
      console.log(config)
      state.config = config
    }
  }
})

new Vue({
  el: '#app',
  router,
  store,
  render: h => h(App)
})
