<template>
  <v-app class="bg-surface">
    <v-app-bar>
      <v-img
        v-if="!isDark"
        src="@/assets/logo.svg"
        max-height="54"
        max-width="54"
        contain
        class="ml-4"
      />
      <v-img
        v-if="isDark"
        src="@/assets/logo_dark.svg"
        max-height="54"
        max-width="54"
        contain
        class="ml-4"
      />
      <v-toolbar-title class="mx-1 text-h5 font-weight-bold">
        Hatch Minty Fresh
      </v-toolbar-title>
      
      <auth-handler/>

    </v-app-bar>

    <v-main class="grey lighten-3 mt-2">
      <v-container>
        <v-row>
          <v-col cols="12" sm="3" class="py-0">
            <target-settings />
          </v-col>

          <v-col cols="12" sm="9" class="py-0">
            <atmosphere-card title="Current Incubator Atmosphere" db-path="IncubatorAtmosphere" />
          </v-col>
        </v-row>
      </v-container>
    </v-main>
  </v-app>
</template>

<script>
import AtmosphereCard from "@/components/AtmosphereCard.vue"
import TargetSettings from "@/components/TargetSettings.vue"
import AuthHandler from "@/components/AuthHandler.vue"

export default {
  name: "App",
  computed: {
    isDark() {
      return this.$vuetify.theme.current.dark;
    },
  },
  components: {
    AtmosphereCard,
    TargetSettings,
    AuthHandler,
  },
}
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

html { overflow-y: auto !important}
</style>