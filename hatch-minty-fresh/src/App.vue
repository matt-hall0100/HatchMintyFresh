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
        Minty Fresh Hatchery
      </v-toolbar-title>
      <v-spacer/>
      <v-btn @click="toggleTheme" icon="mdi-brightness-6"></v-btn>
    </v-app-bar>

    <v-main class="grey lighten-3">
      <v-container>
        <v-row>
          <v-col cols="12" sm="4">

            <v-card class="bg-background elevation-8 mb-6">
              <v-img
                src="https://bdn-ss-hh.s3.amazonaws.com/uploads/2019/08/BabyQuail4.jpg"
                class="align-end"
                gradient="to bottom, #00000000, #00000000 80%, #000000bb"
                cover
              >
                <v-card-title class="text-white">
                    <v-icon color="red" icon="mdi-circle-medium"/>Live
                </v-card-title>
              </v-img>
            </v-card>


            <v-card class="bg-background elevation-8 mb-6">
              <v-card-item title="Current Target Settings"> </v-card-item>
              <v-card-text class="pb-0">
                <v-row no-gutters>
                  <v-col
                    class="text-h4 text-grey-darken-3"
                    cols="6"
                  >
                    <v-icon color="primary" icon="mdi-thermometer"/>102&deg;F
                  </v-col>
                </v-row>
                <v-row no-gutters>
                  <v-col
                    class="text-h4 text-grey-darken-3"
                    cols="6"
                  >
                    <v-icon color="secondary" icon="mdi-water"/>45%
                  </v-col>
                </v-row>
              </v-card-text>
              <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn color="primary" icon="mdi-cog"></v-btn>
              </v-card-actions>
            </v-card>

          </v-col>

          <v-col cols="12" sm="8">
            <AtmosphereCard
              title="Incubator Atmosphere"
              :current-humidity="46.1"
              :current-temperature="100.1"
              :start-open="true"
            />
            <AtmosphereCard
              title="External Atmosphere"
              :current-humidity="46.1"
              :current-temperature="70.2"
            />
          </v-col>
        </v-row>
      </v-container>
    </v-main>
  </v-app>
</template>

<script>
import AtmosphereCard from "@/components/AtmosphereCard.vue"
import { useTheme } from 'vuetify'

export default {
  setup () {
    const theme = useTheme()

    return {
      theme,
      toggleTheme: () => theme.global.name.value = theme.global.current.value.dark ? 'lightTheme' : 'darkTheme'
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