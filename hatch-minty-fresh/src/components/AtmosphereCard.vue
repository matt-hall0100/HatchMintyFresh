<template>
  <v-card class="bg-background elevation-8 mb-6 rounded-xl pa-1">
    <v-card-item :title="title"> </v-card-item>
    <v-card-text class="pb-0">
      <v-row no-gutters>
        <v-col class="text-h5 text-primary">
          <v-icon color="primary" icon="mdi-thermometer" />
          <span v-if="!loading">{{
            parseFloat(currentTemperature.toFixed(1))
          }}&deg;F</span>
          <line-loader v-else width="70px"/>
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col class="text-h5 text-secondary">
          <v-icon color="secondary" icon="mdi-water" />
          <span v-if="!loading">
            {{ parseFloat(currentHumidity.toFixed(1)) }}%
          </span>
          <line-loader v-else width="70px"/>
        </v-col>
      </v-row>
    </v-card-text>

    <div id="chart" class="ma-2">
      <apexchart
        type="area"
        height="350"
        ref="realtimeChart"
        :options="chartOptions"
        :series="series"
      ></apexchart>
    </div>
  </v-card>
</template>

<script>
import { db } from "@/plugins/firebase"
import { ref, onValue} from "firebase/database"
import LineLoader from "@/components/LineLoader.vue"

export default {
  name: "AtmosphereCard",
  props: {
    title: String,
    dbPath: String,
  },
  mounted() {
    this.setDataLineChart();
  },
  components: {
    LineLoader,
  },
  methods: {
    setDataLineChart() {
      const dpRef = ref(db, this.dbPath);
      onValue(dpRef, (snapshot) => {
        const data = snapshot.val();
        const tempData = data.temp;
        const humData = data.hum;

        var latestTemp = Object.keys(tempData).reduce(function(a, b){ return a > b ? a : b })
        var latestHum = Object.keys(humData).reduce(function(a, b){ return a > b ? a : b })
        this.currentTemperature = tempData[latestTemp]
        this.currentHumidity = humData[latestHum]

        var updatedTemp = []
        var updatedHum = []
        var dt = new Date(0)
        var ofst = dt.getTimezoneOffset() * 60;

        for (const [key, value] of Object.entries(tempData)) {
          updatedTemp.push({x: new Date(0).setUTCSeconds(parseInt(key) + ofst), y: value})
        }

        for (const [key, value] of Object.entries(humData)) {
          updatedHum.push({x: new Date(0).setUTCSeconds(parseInt(key) + ofst), y: value})
        }

        this.series[1].data = updatedHum
        this.series[0].data = updatedTemp
        if(this.$refs.realtimeChart) {
          this.$refs.realtimeChart.updateSeries(this.series, true, true);
        }
        this.loading = false
      });
    },
  },
  data: () => ({
    loading: true,

    currentTemperature: 0,
    currentHumidity: 0,

    series: [
      {
        name: "Temperature",
        data: [],
      },
      {
        name: "Humidity",
        data: [],
      },
    ],

    chartOptions: {
      colors: ['#00e396', '#008ffb', '#546E7A', '#E91E63', '#FF9800'],
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
            zoom: true,
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
          max: 110,
          min: 55,
          opposite: true,
          axisTicks: {
            show: true,
            color: "#00e396",
          },
          axisBorder: {
            show: true,
            color: "#00e396",
          },
          title: {
            text: "Temperature (°F)",
            style: {
              color: "#00e396",
            },
          },
          labels: {
            style: {
              colors: "#00e396",
              fontWeight: 600,
            },
            formatter: (v) => { return Math.floor(v) },
          },
        },
        {
          max: 65,
          min: 20,
          axisTicks: {
            show: true,
            color: "#008ffb",
          },
          axisBorder: {
            show: true,
            color: "#008ffb",
          },
          title: {
            text: "Humidity (%)",
            style: {
              color: "#008ffb",
            },
          },
          labels: {
            style: {
              colors: "#008ffb",
              fontWeight: 600,
            },
            formatter: (v) => { return Math.floor(v) },
          },
        },
      ],
      tooltip: {
        shared: true,
        theme: "dark",
        x: {
          format: "MMM dd yyyy h:mm:ss TT",
        },
        y: {
            formatter: (value) => {return parseFloat(value.toFixed(1))},
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