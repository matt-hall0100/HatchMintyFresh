<template>
  <span>
    <v-tooltip
      location="bottom"
      text="Toggle theme"
      open-delay="750"
    >
      <template v-slot:activator="{ props }">
        <v-btn v-bind="props" @click="toggleTheme()" icon="mdi-brightness-6"></v-btn>
      </template>
    </v-tooltip>

    <span v-cloak>
      <span v-if="!user">
        <v-btn
          v-cloak
          class="mx-2"
          @click="signIn()"
          v-if="!this.$vuetify.display.smAndDown"
          variant="tonal"
          prepend-icon="mdi-login"
        >
          Sign In
        </v-btn>
        <v-btn
          class="mr-2"
          @click="signIn()"
          v-if="this.$vuetify.display.smAndDown"
          icon="mdi-login"
        ></v-btn>
      </span>
      <v-menu open-on-hover open-delay="25" :close-on-content-click="false" v-if="user">
        <template v-slot:activator="{ props }">
          <v-avatar class="mx-2" v-bind="props">
            <v-img :src="user.photoURL"></v-img>
          </v-avatar>
        </template>

        <v-card class="rounded-xl" min-width="300px">
          <v-list class="pa-2">
            <v-list-item
              class="rounded-t-xl rounded-b mb-1 pa-3"
              variant="tonal"
              lines="three"
            >
              <template v-slot:prepend>
                <v-avatar
                  class="mt-1"
                  size="x-large"
                  :image="user.photoURL"
                ></v-avatar>
              </template>

              <v-list-item-title>{{
                user.displayName.split("/d/")[0]
              }}</v-list-item-title>

              <v-list-item-subtitle>{{ user.email }}</v-list-item-subtitle>
            </v-list-item>

            <v-btn
              class="rounded-b-xl rounded-t w-100 sign-out-btn"
              @click="signOut()"
              variant="tonal"
              prepend-icon="mdi-login"
              color="grey-darken"
            >
              Sign Out
            </v-btn>

            <v-btn
              variant="plain"
              color="onBackground"
              @click="deleteUser()"
              class="
                d-flex
                px-0
                ma-auto
                text-none
                font-weight-light
                text-caption
              "
            >
              Delete account
            </v-btn>
          </v-list>
        </v-card>
      </v-menu>
    </span>
  </span>
</template>

<script>
import { useTheme } from "vuetify";
import {
  getAuth,
  signInWithPopup,
  signOut,
  deleteUser,
  GoogleAuthProvider,
  onAuthStateChanged,
  updateProfile,
} from "firebase/auth";

const provider = new GoogleAuthProvider();
const auth = getAuth();

export default {
  name: "AuthHandler",
  setup() {
    const theme = useTheme();

    return {
      theme,
      toggleTheme: () => {
        theme.global.name.value = theme.global.current.value.dark
          ? "lightTheme"
          : "darkTheme";
        document
          .querySelector('meta[name="theme-color"]')
          .setAttribute("content", theme.current.value.colors.surface);

        if (theme.global.current.value.dark) {
          localStorage.setItem("theme", "dark");
        } else {
          localStorage.setItem("theme", "light");
        }

        var user = getAuth().currentUser;
        if (user) {
          var name = user.displayName;
          name = name.split("/d/");
          name = name.join("");
          if (theme.global.current.value.dark) {
            updateProfile(auth.currentUser, {
              displayName: name + "/d/",
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
    };
  },
  data: () => ({
    deleteAccountDialog: false,
    user: null,
    displayName: "",
  }),
  mounted() {
    if (localStorage.getItem("theme") == "dark") {
      this.toggleTheme();
    }
    onAuthStateChanged(auth, (user) => {
      if (user != null) {
        this.user = user;
        if (
          user.displayName.includes("/d/") &&
          !this.$vuetify.theme.current.dark
        ) {
          this.toggleTheme();
        } else if (
          !user.displayName.includes("/d/") &&
          this.$vuetify.theme.current.dark
        ) {
          this.toggleTheme();
        }
      } else {
        this.user = null;
      }
    });
  },
  methods: {
    signIn() {
      signInWithPopup(auth, provider)
        .then(() => {})
        .catch((error) => {
          console.log(error);
        });
    },
    signOut() {
      console.log(auth);
      this.user = null;
      signOut(auth);
      console.log(auth);
    },
    deleteUser() {
      deleteUser(this.user)
        .then(() => {})
        .catch((error) => {
          console.log(error);
        });
    },
  },
};
</script>

<style>
.sign-out-btn:hover {
  color: rgb(var(--v-theme-primary));
}
</style>