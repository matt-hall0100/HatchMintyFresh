<template>


  <v-card class="bg-background elevation-8 mb-6">
    <v-card-item title="Current Target Settings"> </v-card-item>
    <v-card-text class="pb-0">
      <v-row no-gutters>
        <v-col
          class="text-h4 text-primary w-100"
        >
          <v-icon color="primary" icon="mdi-thermometer"/>{{target.temperature}}&deg;F
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col
          class="text-h4 text-secondary"
        >
          <v-icon color="secondary" icon="mdi-water"/>{{target.humidity}}%
        </v-col>
      </v-row>
    </v-card-text>
    <v-card-actions>
      <v-spacer></v-spacer>

      <v-dialog
        v-model="dialog"
        fullscreen
        :scrim="false"
        transition="dialog-bottom-transition"
        class="mt-5 elevation-10"
      >
        <template v-slot:activator="{ props }">
          <v-btn color="primary" icon="mdi-cog" v-bind="props"></v-btn>
        </template>
        <v-card class="rounded-xl" color="background">
          <v-toolbar color="background">
            <v-btn icon color="primary" @click="dialog = false">
              <v-icon>mdi-arrow-left</v-icon>
            </v-btn>
            <v-toolbar-title class="text-h5 bg-background"
              >Settings</v-toolbar-title
            >
          </v-toolbar>

          <v-list bg-color="background">
            <v-list-item prepend-icon="mdi-thermometer">
              <v-list-item-title>Target Temperature</v-list-item-title>
              <v-list-item-subtitle class="pa-0">
                <v-slider
                  v-model="target.temperature"
                  :max="110"
                  :min="70"
                  :step="0.1"
                  :thumb-size="14"
                  hide-details
                  class="my-0"
                >
                  <template v-slot:append>
                    <v-text-field
                      v-model="target.temperature"
                      type="number"
                      style="width: 80px"
                      :step="0.1"
                      density="compact"
                      hide-details
                      variant="outlined"
                    ></v-text-field>
                  </template>
                </v-slider>
              </v-list-item-subtitle>
            </v-list-item>

            <v-list-item prepend-icon="mdi-water">
              <v-list-item-title>Target Humidity</v-list-item-title>

              <v-list-item-subtitle class="pa-0">
                <v-slider
                  v-model="target.humidity"
                  :max="60"
                  :min="35"
                  :step="0.1"
                  :thumb-size="14"
                  hide-details
                  class="my-0"
                  color="secondary"
                >
                  <template v-slot:append>
                    <v-text-field
                      v-model="target.humidity"
                      type="number"
                      style="width: 80px"
                      :step="0.1"
                      density="compact"
                      hide-details
                      variant="outlined"
                    ></v-text-field>
                  </template>
                </v-slider>
              </v-list-item-subtitle>
            </v-list-item>

            <v-list-subheader class="mt-6">Advanced Settings</v-list-subheader>
            <v-list-group value="Users">
              <template v-slot:activator="{ props }">
                <v-list-item
                  v-bind="props"
                  title="PID controller"
                  subtitle="Adjust the proportional-integral-derivative controller values"
                  lines="two"
                ></v-list-item>
              </template>

              <v-list-item prepend-icon="mdi-multiplication">
                <v-list-item-title>Proportional</v-list-item-title>

                <v-list-item-subtitle class="pa-0">
                  <v-slider
                    v-model="pid.p"
                    :max="1"
                    :min="0"
                    :step="0.01"
                    :thumb-size="14"
                    hide-details
                    class="my-0"
                    color="onBackground"
                  >
                    <template v-slot:append>
                      <v-text-field
                        v-model="pid.p"
                        type="number"
                        style="width: 80px"
                        :step="0.01"
                        density="compact"
                        hide-details
                        variant="outlined"
                      ></v-text-field>
                    </template>
                  </v-slider>
                </v-list-item-subtitle>
              </v-list-item>

              <v-list-item prepend-icon="mdi-math-integral">
                <v-list-item-title>Integral</v-list-item-title>

                <v-list-item-subtitle class="pa-0">
                  <v-slider
                    v-model="pid.i"
                    :max="1"
                    :min="0"
                    :step="0.01"
                    :thumb-size="14"
                    hide-details
                    class="my-0"
                    color="onBackground"
                  >
                    <template v-slot:append>
                      <v-text-field
                        v-model="pid.i"
                        type="number"
                        style="width: 80px"
                        :step="0.01"
                        density="compact"
                        hide-details
                        variant="outlined"
                      ></v-text-field>
                    </template>
                  </v-slider>
                </v-list-item-subtitle>
              </v-list-item>

              <v-list-item prepend-icon="mdi-delta">
                <v-list-item-title>Derivative</v-list-item-title>

                <v-list-item-subtitle class="pa-0">
                  <v-slider
                    v-model="pid.d"
                    :max="1"
                    :min="0"
                    :step="0.01"
                    :thumb-size="14"
                    hide-details
                    class="my-0"
                    color="onBackground"
                  >
                    <template v-slot:append>
                      <v-text-field
                        v-model="pid.d"
                        type="number"
                        style="width: 80px"
                        :step="0.01"
                        density="compact"
                        hide-details
                        variant="outlined"
                      ></v-text-field>
                    </template>
                  </v-slider>
                </v-list-item-subtitle>
              </v-list-item>

            </v-list-group>
          </v-list>
        </v-card>
      </v-dialog>
    </v-card-actions>
  </v-card>
</template>

<script>
export default {
  name: "TargetSettings",
  data() {
    return {
      dialog: false,
      target: {
        temperature: 95.5,
        humidity: 45.5,
      },
      pid: {
        p: 0.5,
        i: 0.5,
        d: 0.5,
      }
    };
  },
};
</script>

<style>
.dialog-bottom-transition-enter-active,
.dialog-bottom-transition-leave-active {
  transition: transform 0.25s ease-in-out;
}
</style>