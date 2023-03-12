<template>
  <v-app class="bg-surface">
    <v-navigation-drawer
      disable-resize-watcher
      disable-route-watcher
      class="bg-background"
      v-model="drawer"
    >
    </v-navigation-drawer>

    <v-app-bar>
      <v-app-bar-nav-icon @click="drawer = !drawer"></v-app-bar-nav-icon>
      <v-img
        v-if="!isDark"
        src="@/assets/logo.svg"
        max-height="64"
        max-width="64"
        contain
      />
      <v-img
        v-if="isDark"
        src="@/assets/logo_dark.svg"
        max-height="64"
        max-width="64"
        contain
      />
      <v-toolbar-title class="ml-1 text-h5 font-weight-bold">
        Hatch Minty Fresh
      </v-toolbar-title>
      <v-btn @click="toggleTheme()" icon="mdi-brightness-6"></v-btn>
    </v-app-bar>

    <v-main class="grey lighten-3">
      <v-container>
        <v-row>
          <v-col cols="12" sm="4" class="py-0">
            <TargetSettings/>
            
            <AtmosphereCard
              title="External Atmosphere"
            />

          </v-col>

          <v-col cols="12" sm="8" class="py-0">
            <AtmosphereCard
              title="Incubator Atmosphere"
              :start-open="true"
            />
          </v-col>
        </v-row>
      </v-container>
    </v-main>
  </v-app>
</template>

<script>
import AtmosphereCard from "@/components/AtmosphereCard.vue"
import TargetSettings from "@/components/TargetSettings.vue"
import { useTheme } from 'vuetify'

export default {
  setup () {
    const theme = useTheme()

    return {
      theme,
      toggleTheme: () => {
        theme.global.name.value = theme.global.current.value.dark ? 'lightTheme' : 'darkTheme'
        document.querySelector('meta[name="theme-color"]').setAttribute('content',  theme.current.value.colors.surface)
      }
    }
  },
  name: "App",
  data: () => ({
    drawer: false,
  }),
  computed: {
    isDark() {
      return this.$vuetify.theme.current.dark
    }
  },
  components: {
    AtmosphereCard,
    TargetSettings,
  },
};
</script>

<style>
::selection {
  background: rgb(var(--v-theme-secondary));
}
::-moz-selection {
  background: rgb(var(--v-theme-secondary));
}

.v-toolbar-title {
  color: rgb(var(--v-theme-primary));
}
</style>