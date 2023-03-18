import '@mdi/font/css/materialdesignicons.css'
import 'vuetify/styles'
import { md3 } from 'vuetify/blueprints'
import { createVuetify } from 'vuetify'

export const lightTheme = {
  dark: false,
  colors: {
    background: '#ffffff',
    surface: '#fbfffd',
    primary: '#00e396',
    secondary: '#008ffb',
  }
}

export const darkTheme = {
  dark: true,
  colors: {
    background: '#121212',
    surface: '#202222',
    primary: '#00e396',
    secondary: '#008ffb',
  }
}

export default createVuetify({
  theme: {
    defaultTheme: 'lightTheme',
    themes: {
      lightTheme,
      darkTheme,
    }
  },
  blueprint: md3,
})
