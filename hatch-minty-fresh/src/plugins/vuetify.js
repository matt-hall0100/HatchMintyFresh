import '@mdi/font/css/materialdesignicons.css'
import 'vuetify/styles'
import { md3 } from 'vuetify/blueprints'
import { createVuetify } from 'vuetify'

export const lightTheme = {
  dark: false,
  colors: {
    background: '#ffffff',
    surface: '#fbfffd',
    primary: '#3eb489',
    primaryOpaqueFill: '#3eb48944',
    secondary: '#3eaab4',
    secondaryOpaqueFill: '#3eaab444',
  }
}

export const darkTheme = {
  dark: true,
  colors: {
    background: '#121212',
    surface: '#202222',
    primary: '#3eb489',
    primaryOpaqueFill: '#3eb48944',
    secondary: '#3eaab4',
    secondaryOpaqueFill: '#3eaab444',
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
