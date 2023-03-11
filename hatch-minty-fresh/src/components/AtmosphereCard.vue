<template>
  <v-card class="bg-background elevation-8 mb-6">
    <v-card-item :title="title"> </v-card-item>

    <v-card-text class="pb-0">
      <v-row no-gutters>
        <v-col class="text-h4 text-primary" cols="6">
          <v-icon color="primary" icon="mdi-thermometer" />{{
            currentTemperature
          }}&deg;F
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col class="text-h4 text-secondary" cols="6">
          <v-icon color="secondary" icon="mdi-water" />{{ currentHumidity }}%
        </v-col>
      </v-row>
    </v-card-text>

    <v-expand-transition>
      <div v-if="expand">
        <div id="chart">
          <apexchart
            type="area"
            height="350"
            ref="realtimeChart"
            :options="chartOptions"
            :series="series"
          ></apexchart>
        </div>
      </div>
    </v-expand-transition>

    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn
        @click="expand = !expand"
        prepend-icon="mdi-chart-timeline-variant-shimmer"
      >
        {{ !expand ? "Show History" : "Hide History" }}
      </v-btn>
    </v-card-actions>
  </v-card>
</template>

<script>
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
    this.expand = this.startOpen;
    this.setDataLineChart();
  },
  methods: {
    getRandomArbitrary(min, max) {
      return Math.floor((Math.random() * (max - min)) + min);
    },
    setDataLineChart() {
      setInterval(() => {
        this.series[0].data.splice(0, 1);
        this.series[1].data.splice(0, 1);
        this.series[0].data.push({
          x: new Date(),
          y: this.getRandomArbitrary(30, 35),
        });
        this.series[1].data.push({
          x: new Date(),
          y: this.getRandomArbitrary(77, 88),
        });
        this.$refs.realtimeChart.updateSeries(this.series, true, true);
      }, 3000);
    },
  },
  data: () => ({
    expand: false,

    series: [
      {
        name: "Humidity",
        data: [
          { x: new Date().setSeconds(new Date().getSeconds() - 15), y: 31 },
          { x: new Date().setSeconds(new Date().getSeconds() - 12), y: 40 },
          { x: new Date().setSeconds(new Date().getSeconds() - 9), y: 28 },
          { x: new Date().setSeconds(new Date().getSeconds() - 6), y: 51 },
          { x: new Date().setSeconds(new Date().getSeconds() - 3), y: 42 },
          { x: new Date().setSeconds(new Date().getSeconds() - 2), y: 109 },
          { x: new Date().setSeconds(new Date().getSeconds() - 1), y: 100 },
        ],
      },
      {
        name: "Temperature",
        data: [
          { x: new Date().setSeconds(new Date().getSeconds() - 15), y: 11 },
          { x: new Date().setSeconds(new Date().getSeconds() - 12), y: 32 },
          { x: new Date().setSeconds(new Date().getSeconds() - 9), y: 45 },
          { x: new Date().setSeconds(new Date().getSeconds() - 6), y: 32 },
          { x: new Date().setSeconds(new Date().getSeconds() - 3), y: 34 },
          { x: new Date().setSeconds(new Date().getSeconds() - 2), y: 52 },
          { x: new Date().setSeconds(new Date().getSeconds() - 1), y: 41 },
        ],
      },
    ],

    chartOptions: {
      legend: {
        show: true,
        labels: {
          useSeriesColors: true,
        },
      },
      chart: {
        height: 350,
        type: "area",
        toolbar: {
          show: true,
          tools: {
            download: false,
            selection: false,
            zoom: false,
            zoomin: true,
            zoomout: true,
            pan: true,
            reset: true,
          },
        },
      },
      dataLabels: {
        enabled: false,
      },
      stroke: {
        curve: "smooth",
      },
      xaxis: {
        type: "datetime",
        labels: {
          datetimeFormatter: {
            year: "yyyy",
            month: "MMM 'yy",
            day: "dd MMM",
            hour: "h:mm TT",
          },
          style: {
            colors: "#a0a0a0",
          },
        },
      },
      yaxis: [
        {
          max: 75,
          min: 25,
          axisTicks: {
            show: true,
          },
          axisBorder: {
            show: true,
          },
          title: {
            text: "Humidity (%)",
            style: {
              color: "#008ffb",
            },
          },
          labels: {
            style: {
              colors: "#a0a0a0",
            },
          },
        },
        {
          max: 110,
          min: 55,
          opposite: true,
          axisTicks: {
            show: true,
          },
          axisBorder: {
            show: true,
          },
          title: {
            text: "Temperature (°F)",
            style: {
              color: "#00e396",
            },
          },
          labels: {
            style: {
              colors: "#a0a0a0",
            },
          },
        },
      ],
      tooltip: {
        shared: true,
        x: {
          format: "MMM dd yyyy h:mm:ss TT",
        },
      },
      grid: {
        xaxis: {
          lines: {
            show: true,
          },
        },
      },
    },
  }),
};
</script>