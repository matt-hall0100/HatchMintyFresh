<template>
    <span class="skeleton" :style="style"></span>
</template>

<script>
export default {
    name: "LineLoader",
    props: {
      width: String,
    },
    data: () => ({
      style: "",
    }),
    mounted() {
      this.style = "width: " + this.width
    },
}
</script>

<style>
.skeleton {
  display: inline-block;
  border-radius: 4px;
  background: rgba(var(--v-theme-on-background), 0.1);
  background: linear-gradient(to right, rgba(var(--v-theme-on-background), 0.1) 8%, rgba(var(--v-theme-on-background), 0.3) 18%, rgba(var(--v-theme-on-background), 0.1) 33%);
  background-size: 800px 100px;
  animation: shimmer 2s infinite ease-out;
  min-height: 1em;
  line-height: 1;
  vertical-align: middle;
}
@keyframes shimmer {
  0% {
    background-position: -468px 0;
  }
  100% {
    background-position: 468px 0;
  }
}

</style>