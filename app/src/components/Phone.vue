<template>
  <div id="phone" class="pure-g">
    <div class="pure-form pure-form-aligned" id="keypad">
      <fieldset>
        <div id="phone-data">
          <div id="operador">{{ data.cop ? data.cop : 'Sin red' }}</div>
          <div id="signal">
              <ul id="signal-strength">
                  <li class="very-weak" v-if="data.csq"><div></div></li>
                  <li class="weak" v-if="data.csq >= 9"><div></div></li>
                  <li class="strong" v-if="data.csq >= 15"><div></div></li>
                  <li class="pretty-strong" v-if="data.csq >= 19"><div></div></li>
              </ul>
          </div>
        </div>
        <div class="phone-display">
          <div class="loader-container" v-if="calling">
              <div class="loader"></div>
              <div class="loader-txt">Llamando a {{ number }}</div>
          </div>
          <label v-if="response">{{ response }}</label>
          <input type="tel" v-model="number" id="phonenumber" name="number" placeholder="Teléfono" v-if="!calling">
        </div>
        <div class="keypad-buttons">
          <button v-for="b of buttons" :key="b" class="pure-button pure-u-8-24" @click="addNumber(b)">{{ b }}</button>
        </div>
        <div class="phone-controls">
          <div class="pure-button-group" role="group" aria-label="...">
            <button class="pure-button pure-button-success button-success" @click="sendCall" v-if="!calling">Llamar</button>
            <button class="pure-button pure-button-error button-error" @click="hangCall" v-if="calling">Cortar</button>
          </div>
        </div>
      </fieldset>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Message',
  data () {
    return {
      buttons: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'],
      calling: false,
      number: '',
      response: null,
      data: {}
    }
  },
  mounted () {
    if (this.$store.config) {
      this.data = this.$store.config.gsm
    }
  },
  methods: {
    addNumber (n) {
      this.number += '' + n
    },
    sendCall () {
      let self = this
      this.calling = true
      this.response = null
      console.log('Llamando a: ' + this.number)
      this.response = null
      this.$http.post(this.$store.endpoint + 'call',
        { phoneno: this.number },
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          console.log(response)
          self.response = response.data.response
          this.calling = false
        }, response => {
          console.log(response)
          self.response = 'Error al realizar llamada'
          this.calling = false
        })
    },
    hangCall () {
      this.calling = false
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
#keypad fieldset{
    padding: 10px;
    position: relative;
}
#phonenumber{
    width: 100%;
}
#phone fieldset{
    position: relative;
}

#phone-data{
    height: 24px;
    background: #828282;
    letter-spacing: initial;
    color:#fff;
    padding: 3px 10px 3px 10px;
    overflow: hidden;
}

#operador{
    line-height: 25px;
}

#signal{
    float: right;
    top: 13px;
    position: absolute;
    right: 15px;
}

#signal-strength {
  height: 20px;
  width: 30px;
  list-style: none;
  overflow: hidden;
  margin: 0;
  padding: 0;
  float: right;
}
#signal-strength li {
  display: inline-block;
  width: 6px;
  float: left;
  height: 100%;
  margin-right: 1px;
}
#signal-strength li.pretty-strong {
  padding-top: 0px;
}
#signal-strength li.strong {
  padding-top: 5px;
}
#signal-strength li.weak {
  padding-top: 9px;
}
#signal-strength li.very-weak {
  padding-top: 15px;
}
#signal-strength li div {
  height: 100%;
  background: #fff;
}
#phonenumber{
  text-align: center;
  height: 100px;
  border: none;
  box-shadow: none;
  margin: 0;
  border-radius: 0;
}
.phone-display{
  height: 100px;
  position: relative;
  background: #fff;
  letter-spacing: initial;
  text-align: center;
  overflow: hidden;
}

.phone-controls button{
  width: 50%;
}
</style>
