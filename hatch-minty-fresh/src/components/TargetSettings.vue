<template>
  <v-card class="bg-background elevation-8 mb-6 rounded-xl pa-1">
    <v-card-item title="Target Settings"> </v-card-item>
    <v-card-text>
      <v-row no-gutters>
        <v-col class="text-h4 text-primary w-100">
          <v-icon color="primary" icon="mdi-thermometer" />
          <span v-if="!loadingTemp">{{ target.temperature }}&deg;F</span>
          <line-loader v-else width="100px" />
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col class="text-h4 text-secondary">
          <v-icon color="secondary" icon="mdi-water" />
          <span v-if="!loadingHum">{{ target.humidity }}%</span>
          <line-loader v-else width="100px" />
        </v-col>
      </v-row>
    </v-card-text>
    <v-card-actions v-cloak v-if="user" class="pt-0">
      <v-spacer></v-spacer>

      <v-dialog
        v-model="dialog"
        fullscreen
        :scrim="false"
        transition="dialog-bottom-transition"
        class="mt-5"
        close-on-back
      >
        <template v-slot:activator="{ props }">
          <v-btn color="primary" icon="mdi-cog" v-bind="props"></v-btn>
        </template>
        <v-card
          v-touch="{ down: () => (dialog = false) }"
          class="rounded-xl mt-5 elevation-10"
          color="background"
        >
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
                  @touchstart.stop
                  :disabled="!allowedUsers.includes(user.email)"
                  v-model="target.temperature"
                  :max="105"
                  :min="70"
                  :step="0.1"
                  :thumb-size="14"
                  hide-details
                  class="my-0"
                  @update:model-value="updateTargetTemperature()"
                >
                  <template v-slot:append>
                    <v-text-field
                      v-model="target.temperature"
                      type="number"
                      style="width: 110px"
                      :step="0.1"
                      density="compact"
                      hide-details
                      variant="outlined"
                      suffix="&deg;F"
                      @update:model-value="updateTargetTemperature()"
                    ></v-text-field>
                  </template>
                </v-slider>
              </v-list-item-subtitle>
            </v-list-item>

            <v-list-item prepend-icon="mdi-water">
              <v-list-item-title>Target Humidity</v-list-item-title>

              <v-list-item-subtitle class="pa-0">
                <v-slider
                  @touchstart.stop
                  :disabled="!allowedUsers.includes(user.email)"
                  v-model="target.humidity"
                  :max="60"
                  :min="30"
                  :step="0.1"
                  :thumb-size="14"
                  hide-details
                  class="my-0"
                  color="secondary"
                  @update:model-value="updateTargetHumidity()"
                >
                  <template v-slot:append>
                    <v-text-field
                      v-model="target.humidity"
                      type="number"
                      style="width: 110px"
                      :step="0.1"
                      density="compact"
                      hide-details
                      variant="outlined"
                      suffix="%"
                      @update:model-value="updateTargetHumidity()"
                    ></v-text-field>
                  </template>
                </v-slider>
              </v-list-item-subtitle>
            </v-list-item>

            <v-list-subheader class="mt-6">Advanced Settings</v-list-subheader>
            <v-list-group value="Allowed users" active-color="primary">
              <template v-slot:activator="{ props }">
                <v-list-item
                  v-bind="props"
                  title="Allowed users"
                  subtitle="The group of users allowed to make setting adjustmenets"
                  lines="two"
                  prepend-icon="mdi-account-group"
                ></v-list-item>
              </template>

              <v-list-item>
                <v-combobox
                  variant="outlined"
                  v-model="allowedUsers"
                  hint="Enter emails of trusted accounts"
                  @update:model-value="verifyCombo()"
                  chips
                  multiple
                  auto-grow
                  :disabled="!allowedUsers.includes(user.email)"
                >
                  <template v-slot:chip="{ item }">
                    <v-tooltip
                      location="bottom"
                      :disabled="item.value != user.email"
                      text="You cannot remove yourself from this list"
                    >
                      <template v-slot:activator="{ props }">
                        <v-chip
                          class="rounded-pill"
                          v-bind="props"
                          :closable="item.value != user.email"
                          @click:close="
                            allowedUsers.splice(
                              allowedUsers.indexOf(item.value),
                              1
                            );
                            verifyCombo();
                          "
                        >
                          {{ item.value }}
                        </v-chip>
                      </template>
                    </v-tooltip>
                  </template>
                </v-combobox>
              </v-list-item>
            </v-list-group>
            <v-list-item
              title="Save data"
              subtitle="Download all chart temperature and humidity history"
              lines="two"
              prepend-icon="mdi-content-save-all"
              @click="saveData()"
            ></v-list-item>
            <v-dialog v-model="deleteDialog" width="auto">
              <template v-slot:activator="{ props }">
                <v-list-item
                  v-bind="props"
                  title="Delete data"
                  subtitle="Erase all chart temperature and humidity history"
                  lines="two"
                  prepend-icon="mdi-trash-can-outline"
                  :disabled="!allowedUsers.includes(user.email)"
                ></v-list-item>
              </template>
              <v-card class="rounded-xl pa-1" max-width="650px">
                <v-card-title
                  ><v-icon color="error" icon="mdi-alert" /><span class="pa-4"
                    >Warning: Irreversible Action</span
                  ></v-card-title
                >
                <v-card-text>
                  You are about to delete all historical temperature and
                  humidity data currently saved in the database. This data
                  cannot be recovered once deleted. Are you sure you want to
                  delete all data?
                </v-card-text>
                <v-card-actions>
                  <v-row class="ma-0 pa-0">
                    <v-col>
                      <v-btn
                        variant="tonal"
                        color="error"
                        block
                        @click="deleteAtmHist()"
                        >Confirm Deletion</v-btn
                      >
                    </v-col>
                    <v-col>
                      <v-btn
                        variant="tonal"
                        color="primary"
                        block
                        @click="deleteDialog = false"
                        >Cancel</v-btn
                      >
                    </v-col>
                  </v-row>
                </v-card-actions>
              </v-card>
            </v-dialog>
            <v-list-item
              title="Advanced statistics"
              subtitle="Enable stats for nerds 🤓"
              lines="two"
              prepend-icon="mdi-chart-bar-stacked"
              @click="toggleNerd()"
            >
              <template v-slot:append>
                <v-switch
                  @click="null"
                  density="compact"
                  inset
                  hide-details
                  v-model="nerd"
                  color="primary"
                ></v-switch>
              </template>
            </v-list-item>
          </v-list>
        </v-card>
      </v-dialog>
    </v-card-actions>
  </v-card>
</template>

<script>
import { db } from "@/plugins/firebase";
import { ref, onValue, set, remove, get } from "firebase/database";
import { getAuth, onAuthStateChanged, updateProfile } from "firebase/auth";
import LineLoader from "@/components/LineLoader.vue";

const auth = getAuth();
const allowedUsersRef = ref(db, "AllowedUsers");
const targetHumRef = ref(db, "Configurable/TargetHum");
const targetTempRef = ref(db, "Configurable/TargetTemp");
const tempKpRef = ref(db, "PIDTuning/tempKp");
const tempKiRef = ref(db, "PIDTuning/tempKi");
const tempKdRef = ref(db, "PIDTuning/tempKd");
const atmRef = ref(db, "IncubatorAtmosphere");

export default {
  name: "TargetSettings",
  data: () => ({
    dialog: false,
    deleteDialog: false,
    target: {
      temperature: null,
      humidity: null,
    },
    nerd: false,
    pid: {
      p: 0.5,
      i: 0.5,
      d: 0.5,
    },
    user: null,
    allowedUsers: [],
    loadingTemp: true,
    loadingHum: true,
  }),
  components: {
    LineLoader,
  },
  mounted() {
    onAuthStateChanged(auth, (user) => {
      if (user != null) {
        this.user = user;
        if (user.displayName.includes(" 🤓")) {
          this.nerd = true
        }
        else if (!user.displayName.includes(" 🤓")) {
          this.nerd = false
        }
      } else {
        this.user = null;
      }
    });
    onValue(allowedUsersRef, (snapshot) => {
      this.allowedUsers = snapshot.val().emails.split(",");
    });
    onValue(targetHumRef, (snapshot) => {
      this.target.humidity = snapshot.val();
      this.loadingHum = false;
    });
    onValue(targetTempRef, (snapshot) => {
      this.target.temperature = snapshot.val();
      this.loadingTemp = false;
    });
    onValue(tempKpRef, (snapshot) => {
      this.pid.p = snapshot.val();
    });
    onValue(tempKiRef, (snapshot) => {
      this.pid.i = snapshot.val();
    });
    onValue(tempKdRef, (snapshot) => {
      this.pid.d = snapshot.val();
    });
  },
  methods: {
    verifyCombo() {
      if (this.allowedUsers.length) {
        if (
          !/^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$/.test(
            this.allowedUsers.at(-1)
          )
        ) {
          var last = this.allowedUsers.pop().replace(/\s+/g, "").split(",");
          var allGood = true;
          last.forEach((email) => {
            if (
              !/^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$/.test(email)
            ) {
              allGood = false;
            }
          });
          if (allGood) {
            last.forEach((email) => {
              if (!this.allowedUsers.includes(email)) {
                this.allowedUsers = this.allowedUsers.concat(email);
              }
            });
          }
        }
      }

      if (!this.allowedUsers.includes(this.user.email)) {
        this.allowedUsers = this.allowedUsers.concat(this.user.email);
      }

      set(allowedUsersRef, { emails: this.allowedUsers.join(",") });
    },
    deleteAtmHist() {
      remove(atmRef);
      this.deleteDialog = false;
    },
    saveData() {
      get(atmRef)
        .then((snapshot) => {
          if (snapshot.exists()) {
            var out = ["Date/Time,\tTemperature (F),\tHumidity (%),\tTarget Temperature (F),\tTarget Humidity (%),\tTemperature Intensity,\tHumidity Off/On"];

            for (const [key, value] of Object.entries(snapshot.val())) {
              const date = new Date(key * 1000);
              var str = date.toString() + ",\t";
              str = str + value[0].toString() + ",\t";
              str = str + value[1].toString() + ",\t";
              str = str + value[2].toString() + ",\t";
              str = str + value[3].toString() + ",\t";
              str = str + value[4].toString() + ",\t";
              str = str + value[5].toString() + ",\t";
              out.push(str);
            }

            out = out.join("\n");

            // Create and download csv
            const blob = new Blob([out], { type: "text/csv" });
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement("a");
            a.setAttribute("href", url);
            a.setAttribute("download", "hatch-minty-fresh-history.csv");
            a.click();
          } else {
            console.log("No data available");
          }
        })
        .catch((error) => {
          console.error(error);
        });
    },
    toggleNerd() {
      this.nerd = !this.nerd;
      if (this.user) {
        var name = this.user.displayName;
        name = name.split(" 🤓");
        name = name.join("");
        if (this.nerd) {
          updateProfile(auth.currentUser, {
            displayName: name + " 🤓",
          })
            .then(() => {})
            .catch((error) => {
              console.log(error);
            });
        } else {
          updateProfile(auth.currentUser, {
            displayName: name,
          })
            .then(() => {})
            .catch((error) => {
              console.log(error);
            });
        }
      }
    },
    updateTargetTemperature() {
      set(targetTempRef, parseFloat(this.target.temperature));
    },
    updateTargetHumidity() {
      set(targetHumRef, parseFloat(this.target.humidity));
    },
    updatePidP() {
      set(tempKpRef, this.pid.p);
    },
    updatePidI() {
      set(tempKiRef, this.pid.i);
    },
    updatePidD() {
      set(tempKdRef, this.pid.d);
    },
  },
};
</script>

<style>
.dialog-bottom-transition-enter-active,
.dialog-bottom-transition-leave-active {
  transition: transform 0.25s ease-in-out;
}
</style>