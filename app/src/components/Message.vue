<template>
  <div class="pure-form pure-form-stacked">
    <fieldset>
        <legend>Enviar mensaje <div v-if="loading" style="float:right" class="loader"></div></legend>
        <input type="number" v-model="message.phoneno" name="phoneno" placeholder="Teléfono" class="phoneno">
        <textarea v-model="message.message" name="message" class="message"></textarea>
        <button class="pure-button pure-button-primary sendmessage" @click="sendMsg">Enviar</button>
        <div class="" v-if="message_response">
            <label>Respuesta: </label>{{ message_response }}
        </div>
    </fieldset>
  </div>
</template>

<script>
export default {
  name: 'Message',
  data () {
    return {
      loading: false,
      message: {
        message: '',
        phoneno: ''
      },
      message_response: null
    }
  },
  mounted () {

  },
  methods: {
    sendMsg () {
      let self = this
      this.loading = true
      console.log('Enviar mensaje:')
      console.log(this.message.message)
      console.log('A telefono: ' + this.message.phoneno)
      this.message_response = null
      this.$http.post(this.$store.endpoint + 'msg',
        this.message,
        {
          emulateJSON: true,
          method: 'POST'
        }
      )
        .then(response => {
          console.log(response)
          self.message_response = response.data.response
          self.loading = false
        }, response => {
          console.log(response)
        })
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.phoneno{
  width: 100%;
}
.message{
  width: 80%;
}
.sendmessage{
  width: 19%;
  margin:5px 0 5px 1%;
  float: right;
}
</style>
