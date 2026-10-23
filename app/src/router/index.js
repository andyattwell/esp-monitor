import Vue from 'vue'
import Router from 'vue-router'
import Inicio from '@/components/Inicio'
import Wifi from '@/components/Wifi'
import Gsm from '@/components/Gsm'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'Inicio',
      component: Inicio
    },
    {
      path: '/wifi',
      name: 'Wifi',
      component: Wifi
    },
    {
      path: '/gsm',
      name: 'Gsm',
      component: Gsm
    }
  ]
})
