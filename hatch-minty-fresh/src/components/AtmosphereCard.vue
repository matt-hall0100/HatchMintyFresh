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

        var latest = Object.keys(data).reduce(function(a, b){ return a > b ? a : b })
        this.currentTemperature = data[latest][0]
        this.currentHumidity = data[latest][1]

        var temp = []
        var hum = []
        var tempIntensity = []
        var humOn = []
        var targetTemp = []
        var targetHum = []

        for (const [key, value] of Object.entries(data)) {
          var t = new Date(0).setUTCSeconds(parseInt(key))
          temp.push({x: t, y: value[0]})
          hum.push({x: t, y: value[1]})
          tempIntensity.push({x: t, y: value[2]})
          value[3] ? humOn.push({x: t, y: 1}) : humOn.push({x: t, y: 0})
          targetTemp.push({x: t, y: value[4]})
          targetHum.push({x: t, y: value[5]})
        }

        this.series[0].data = temp
        this.series[1].data = hum
        this.series[2].data = targetTemp
        this.series[3].data = targetHum

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
      {
        name: "Target Temperature",
        data: [],
      },
      {
        name: "Target Humidity",
        data: [],
      },
    ],
    chartOptions: {
      colors: ['#00e396', '#008ffb', '#e3004d', '#fb6c00'],
      stroke: {
        width: [4, 4, 2, 2],
        curve: ['smooth','smooth','stepline','stepline'],
        dashArray: [0, 0, 2, 2]
      },
      fill: {
        type: "solid",
        opacity: [0.35, 0.35, 0, 0]
      },
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
      xaxis: {
        type: "datetime",
        labels: {
          datetimeUTC: false,
          datetimeFormatter: {
              year: 'yyyy',
              month: "MMM 'yy",
              day: 'dd MMM',
              hour: 'h:mm TT',
              minute: 'h:mm:ss TT',
              second: 'h:mm:ss TT'
          },
          style: {
            colors: "#a0a0a0",
          },
        },
      },
      yaxis: [
        {
          max: 105,
          min: 65,
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
        {
          max: 105,
          min: 65,
          show: false,
        },
        {
          max: 65,
          min: 20,
          show: false,
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
    },
  }),
};
</script>