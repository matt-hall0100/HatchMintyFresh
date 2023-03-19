import { createApp } from 'vue'
import App from './App.vue'
import vuetify from './plugins/vuetify'
import { loadFonts } from './plugins/webfontloader'
import VueApexCharts from "vue3-apexcharts";
import './registerServiceWorker'

loadFonts()

createApp(App)
  .use(vuetify)
  .use(VueApexCharts)
  .mount('#app')
