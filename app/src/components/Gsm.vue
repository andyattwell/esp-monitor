<template>
<div>
    <div class="pure-g content-body">
        <div class="pure-u-1 pure-u-6-md-24">
            <phone></phone>
        </div>
        <div class="pure-u-1 pure-u-6-md-24">
             <div class="pure-form pure-form-aligned">
                <message></message>
             </div>
        </div>

        <div class="loader-container" v-if="loading">
            <div class="loader"></div>
        </div>
    </div>
</div>
</template>

<script>
import Message from './Message.vue'
import Phone from './Phone.vue'
export default {
  name: 'Gsm',
  components: {
    'message': Message,
    'phone': Phone
  },
  data () {
    return {
      loading: false,
      data: {}
    }
  },
  mounted () {
    // this.getData()
    if (this.$store.config) {
      this.data = this.$store.config.gsm
    }
    if (self.data && self.data.imei) {
      self.data.connected = true
    }
  },
  computed: {

  },
  methods: {
    getData () {
      let self = this
      this.loading = true
      console.log('Empezó')
      this.$http.get(this.$store.endpoint + 'gsm')
        .then(response => {
          self.data = response.data
          if (self.data.imei) {
            self.data.connected = true
          }
          self.loading = false
          console.log(response.data)
        }, (response) => {
          console.log(response)
        })
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>

</style>
