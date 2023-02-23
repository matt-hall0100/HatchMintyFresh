<template>
  <v-card class="bg-background elevation-8 mb-6">
    <v-card-item :title="title"> </v-card-item>

    <v-card-text class="pb-0">
      <v-row no-gutters>
        <v-col class="text-h4 text-primary" cols="6">
          <v-icon color="primary" icon="mdi-thermometer"/>{{ currentTemperature }}&deg;F
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col class="text-h4 text-secondary" cols="6">
          <v-icon color="secondary" icon="mdi-water"/>{{ currentHumidity }}%
        </v-col>
      </v-row>
    </v-card-text>

    <v-expand-transition>
      <div v-if="expand">
        <Line class="ma-6" :data="data" :options="options" ref="lineChart"/>
      </div>
    </v-expand-transition>

    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn @click="expand = !expand" prepend-icon="mdi-chart-timeline-variant-shimmer">
        {{ !expand ? "Show History" : "Hide History" }}
      </v-btn>
    </v-card-actions>
  </v-card>
</template>

<script>
import { lightTheme } from "@/plugins/vuetify";
import {
  Chart,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler,
} from "chart.js";
import { Line } from "vue-chartjs";

Chart.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
)

export default {
  name: "AtmosphereCard",
  props: {
    title: String,
    currentTemperature: Number,
    currentHumidity: Number,
    startOpen: {
      type: Boolean,
      default: false,
    },
  },
  mounted() {
    this.expand = this.startOpen
    Chart.defaults.borderColor = this.$vuetify.theme.current.colors['on-surface-variant']
    Chart.defaults.color = this.$vuetify.theme.current.colors["on-background"]
    Chart.defaults.font.size = 14
  },
  data: () => ({
    expand: false,
    data: {
      labels: ["90", "80", "70", "60", "50", "40", "30", "20", "10", "0"],
      datasets: [
        {
          label: "Humidity",
          cubicInterpolationMode: "monotone",
          fill: true,
          data: [30.8, 30.9, 31.2, 32.8, 36.4, 41.5, 44.8, 45.7, 45.9, 46.1],
          borderColor: lightTheme.colors.secondary,
          backgroundColor: lightTheme.colors.secondaryOpaqueFill,
          yAxisID: "y1",
          pointRadius: 0,
        },
        {
          label: "Temperature",
          cubicInterpolationMode: "monotone",
          fill: true,
          data: [70.7, 71.3, 76.1, 84.4, 93.1, 97.5, 99.3, 99.9, 100.3, 100.1],
          borderColor: lightTheme.colors.primary,
          backgroundColor: lightTheme.colors.primaryOpaqueFill,
          yAxisID: "y",
          pointRadius: 0,
        },
      ],
    },
    options: {
      plugins: {
        legend: {
          labels: {
            usePointStyle: true,
          },
        },
      },
      responsive: true,
      interaction: {
        mode: "index",
        intersect: false,
      },
      scales: {
        x: {
          title: {
            display: true,
            text: "Minutes Ago",
          },
        },
        y: {
          type: "linear",
          display: true,
          position: "left",
          title: {
            display: true,
            text: "Temperature (°F)",
          },
          min: 65,
          max: 110,
        },
        y1: {
          type: "linear",
          display: true,
          position: "right",
          title: {
            display: true,
            text: "Humidity (%)",
          },
          suggestedMin: 25,
          suggestedMax: 75,

          // grid line settings
          grid: {
            drawOnChartArea: false, // only want the grid lines for one axis to show up
          },
        },
      },
    },
  }),
  components: {
    Line,
  },
};
</script>