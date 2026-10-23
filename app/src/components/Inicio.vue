<template>
  <div class="inicio">

  <div class="content-body">
    <div class="loader-container" v-if="loading">
        <div class="loader"></div>
    </div>
    <table class="pure-table"  style="width: 100%;">
      <thead>
          <tr>
              <th></th>
              <th>Servicio</th>
              <th></th>
              <th></th>
          </tr>
      </thead>

      <tbody>
          <tr class="pure-table-odd">
              <td>
                <div>
                  {{ data.config.apssid ? 'Si' : 'No' }}
                </div>
              </td>
              <td>AP</td>
              <td>{{ data.apip }}</td>
              <td>
                  <div class="pure-button-group" role="group" aria-label="...">
                      <router-link to="/ap" class="pure-button pure-button-primary">Editar</router-link>
                  </div>
              </td>
          </tr>

          <tr>
              <td>
                {{ data.wifiip ? 'Si' : 'No' }}
              </td>
              <td>Wifi</td>
              <td>{{ data.wifiip }}</td>
              <td>
                  <div class="pure-button-group" role="group" aria-label="...">
                      <router-link to="/wifi" class="pure-button">Editar</router-link>
                  </div>
              </td>
          </tr>

          <tr class="pure-table-odd">
              <td>
                {{ data.gsm ? 'Si' : 'No' }}
              </td>
              <td>GSM</td>
              <td>{{ data.gsm ? data.gsm.cop : '' }}</td>
              <td>
                  <div class="pure-button-group" role="group" aria-label="...">
                      <router-link to="/gsm" class="pure-button">Editar</router-link>
                  </div>
              </td>
          </tr>
      </tbody>
    </table>
    <hr>
    <div class="pure-form pure-form-aligned">
      <fieldset>
        <legend>Configuración de AP</legend>
        <div class="pure-control-group">
            <label>SSID</label>
            <input type="text" name="apssid" id="apssid" placeholder="SSID" v-model="data.config.apssid">
        </div>
        <div class="pure-control-group">
          <label>Contraseña</label>
          <input type="password" name="appass" id="appass" placeholder="Contraseña" v-model="data.config.appass">
        </div>
        <div class="pure-control-group">
          <label>AP Timeout</label>
          <input type="number" name="aptimeout" id="aptimeout" placeholder="" v-model="data.config.aptimeout">
        </div>
        <legend>Configuración de Alarmas</legend>
        <div class="pure-control-group">
            <label>Teléfono 1</label>
            <input type="text" name="phoneno" id="phoneno" placeholder="Teléfono" v-model="data.config.phoneno">
        </div>
        <div class="pure-control-group">
            <label>Teléfono 2</label>
            <input type="text" name="phoneno2" id="phoneno2" placeholder="Teléfono" v-model="data.config.phoneno2">
        </div>
        <div class="pure-control-group">
            <label>Mensaje de alerta</label>
            <input type="text" name="alertmsg" id="alertmsg" placeholder="Mensaje que se le enviará" v-model="data.config.alertmsg">
        </div>
      </fieldset>
      <div class="pure-controls form-controls">
        <div class="pure-button-group" role="group" aria-label="...">
            <button type="button" @click="saveConf" class="pure-button pure-button-primary">Guardar</button>
            <button type="button" @click="resetDefault" class="pure-button pure-button-default">Reestablecer</button>
        </div>
      </div>
    </div>
    <wifi v-if="!loading"></wifi>

  </div>
  </div>
</template>

<script>
import Wifi from './Wifi.vue'

export default {
  name: 'Inicio',
  components: {
    'wifi': Wifi
  },
  data () {
    return {
      loading: false
    }
  },
  computed: {
    data () {
      return this.$store.getters.config
    }
  },
  mounted () {
  },
  methods: {
    saveConf () {
      let self = this
      self.loading = true
      console.log(self.data.config)
      this.$http.post(this.$store.getters.endpoint + 'config/save',
        self.data.config,
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          console.log(response)
          self.loading = false
          return false
        }, response => {
          // error callback
          console.log(response)
          self.loading = false
          return false
        })
    },
    resetDefault () {
      this.$http.post(this.$store.endpoint + 'config/reset',
        { data: 1 },
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          console.log(response)
          self.loading = false
          return false
        }, response => {
          // error callback
          console.log(response)
          self.loading = false
          return false
        })
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h1, h2 {
  font-weight: normal;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
.form-controls{
  margin:15px 0;
}
</style>
