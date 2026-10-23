<template>
<div class="pure-form pure-form-aligned">
  <fieldset>
      <legend>Conexión Wifi</legend>
      <div class="pure-control-group">
          <label>Estado:</label>
          {{ estado }}
      </div>
      <div class="pure-control-group">
          <label>Ip:</label>
          {{ this.data.ip }}
      </div>
      <div class="pure-control-group">
          <label for="wifissid">SSID</label>
          <select name="wifissid" id="wifissid" v-model="data.ssid" @change="selectWifi">
              <option></option>
              <option v-for="net of data.available" :key="net._n" :value="net.SSID">{{ net.SSID }}</option>
              <option value="otro">Otro</option>
          </select>
      </div>
      <div class="pure-control-group" v-if="data.ssid == 'otro'">
          <label for="otro">Nombre de SSID</label>
          <input name="otro" v-model="data.ssid_otro">
      </div>
      <div class="pure-control-group">
          <label for="password">Contraseña</label>
          <input type="password" name="wifipass" id="wifipass"  v-model="data.password">
      </div>
      <div class="pure-control-group">
          <label for="password">Autoconectar</label>
          <input type="checkbox" name="wificonnect" id="wificonnect"  v-model="data.connect">
      </div>
      <div class="pure-controls">
          <div class="pure-button-group" role="group" aria-label="...">
              <button class="pure-button pure-button-primary" @click="connectWifi">Conectar</button>
              <button class="pure-button pure-button-default" @click="disconectWifi">Desconectar</button>
          </div>
      </div>
  </fieldset>
  <div class="loader-container" v-if="loading">
      <div class="loader"></div>
  </div>
</div>
</template>

<script>
export default {
  name: 'Wifi',
  props: ['inputdata'],
  data () {
    return {
      loading: false,
      wifiModal: false,
      status_codes: ['Desconectado', 'Error de SSID', 'Error 4 (?)', 'Conectado', 'Error 5 (?)', 'Fallo al conectar']
    }
  },
  mounted () {
  },
  computed: {
    estado () {
      return this.status_codes[this.data.status]
    },
    data () {
      return this.$store.getters.config.wifi
    }
  },
  methods: {
    getData () {
      let self = this
      this.$http.get(this.$store.getters.endpoint + 'wifi')
        .then(response => {
          console.log(response.data)
          if (response.data.connect === 'false') {
            response.data.connect = null
          } else {
            response.data.connect = true
          }
          self.data = response.data
          if (self.data.ssid !== '') {
            let isAv = false
            self.data.available.forEach(function (element) {
              if (element.SSID === self.data.ssid) {
                isAv = true
              }
            }, this)

            if (!isAv) {
              self.data.ssid_otro = self.data.ssid
              self.data.ssid = 'otro'
            }
          }
          self.loading = false
        }, (response) => {
          console.warn('Error consiguiendo data de wifi')
          console.log(response)
          self.loading = false
        })
    },
    connectWifi () {
      console.log('Conectar a red wifi')
      let ssid = this.data.ssid
      let auto = 'false'
      if (this.data.ssid === 'otro') {
        ssid = this.data.ssid_otro
      }
      if (this.data.connect) {
        auto = 'true'
      }

      let wData = {
        wifissid: ssid,
        wifipass: this.data.password,
        wificonnect: auto
      }

      this.$http.post(this.$store.getters.endpoint + 'wifi/save',
        wData,
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          return false
        }, response => {
          // error callback
          console.log(response)
          return false
        })
      this.wifiModal = false
      return false
    },
    disconectWifi () {
      console.log('Desconectar Wifi')
      this.$http.post(this.$store.endpoint + 'wifi/save',
        {
          wifissid: '',
          wifipass: ''
        },
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          console.log(response.data)
          return false
        }, response => {
          // error callback
          console.log(response)
          return false
        })
      this.wifiModal = false
      return false
    },
    selectWifi (data) {
      if (data.target) {
        this.data.ssid = data.target.value
      } else {
        this.data.ssid = data.SSID
      }
      this.wifiModal = true
      this.data.password = null
      return false
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.wifi-list tr.active > td{
    font-weight: bold
}
.content-body .pure-u > div{
    padding: 0 15px;
}
</style>
