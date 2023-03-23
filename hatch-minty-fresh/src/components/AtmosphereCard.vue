<template>
  <v-card class="bg-background elevation-8 mb-6 rounded-xl pa-1">
    <v-card-item :title="title"> </v-card-item>
    <v-card-text class="pb-0">
      <v-row no-gutters>
        <v-col class="text-h5 text-primary">
          <v-icon color="primary" icon="mdi-thermometer" />
          <span v-if="!loading"
            >{{ parseFloat(currentTemperature.toFixed(1)) }}&deg;F</span
          >
          <line-loader v-else width="70px" />
        </v-col>
      </v-row>
      <v-row no-gutters>
        <v-col class="text-h5 text-secondary">
          <v-icon color="secondary" icon="mdi-water" />
          <span v-if="!loading">
            {{ parseFloat(currentHumidity.toFixed(1)) }}%
          </span>
          <line-loader v-else width="70px" />
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
import { db } from "@/plugins/firebase";
import { ref, onValue } from "firebase/database";
import LineLoader from "@/components/LineLoader.vue";
import { getAuth, onAuthStateChanged } from "firebase/auth";
import { LTTB } from "downsample";

const auth = getAuth();
const maxPoints = 850;

export default {
  name: "AtmosphereCard",
  props: {
    title: String,
    dbPath: String,
  },
  mounted() {
    this.setDataLineChart();
    onAuthStateChanged(auth, (user) => {
      if (user != null) {
        this.user = user;
      } else {
        this.user = null;
      }
    });
  },
  components: {
    LineLoader,
  },
  methods: {
    setDataLineChart() {
      const dpRef = ref(db, this.dbPath);
      onValue(dpRef, (snapshot) => {
        const data = snapshot.val();

        var isNerd = this.user && this.user.displayName.includes(" 🤓");

        // Get the most recent values
        var latest = Object.keys(data).reduce(function (a, b) {
          return a > b ? a : b;
        });
        this.currentTemperature = data[latest][0];
        this.currentHumidity = data[latest][1];

        // Get data arrays
        var times = Object.keys(data);
        var temp = times.map((v) => {return { x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][0] }});
        var hum = times.map((v) => {return { x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][1] }});
        var tempIntensity = isNerd ? times.map((v) => {return {x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][2] }}) : null;
        var humOn = isNerd ? times.map((v) => {return {x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][3] ? 0 : 1 }}) : null;
        var targetTemp = times.map((v) => {return { x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][4] }});
        var targetHum = times.map((v) => {return { x: new Date(0).setUTCSeconds(parseInt(v)), y: data[v][5] }});

        // Downsample if needed
        if (times.length > maxPoints) {
          temp = LTTB(temp, maxPoints);
          hum = LTTB(hum, maxPoints);
          tempIntensity = isNerd ? LTTB(tempIntensity, maxPoints) : null;
          humOn = isNerd ? LTTB(humOn, maxPoints) : null;
          targetTemp = LTTB(targetTemp, maxPoints);
          targetHum = LTTB(targetHum, maxPoints);
        }

        // Assign data to the series
        this.series[0].data = temp;
        this.series[1].data = hum;
        this.series[4].data = isNerd ? tempIntensity : null;
        this.series[5].data = isNerd ? humOn : null;
        this.series[2].data = targetTemp;
        this.series[3].data = targetHum;

        this.loading = false;
      });
    },
  },
  data: () => ({
    user: null,
    loading: true,

    currentTemperature: 0,
    currentHumidity: 0,

    series: [
      {
        name: "Temperature",
        data: [],
        type: "area",
      },
      {
        name: "Humidity",
        data: [],
        type: "area",
      },
      {
        name: "Target Temperature",
        data: [],
        type: "area",
      },
      {
        name: "Target Humidity",
        data: [],
        type: "area",
      },
      {
        name: "Temperature Intensity",
        data: [],
        type: "area",
      },
      {
        name: "Humidity Off/On",
        data: [],
        type: "area",
      },
    ],
    chartOptions: {
      colors: [
        "#00e396",
        "#008ffb",
        "#e3004d",
        "#fb6c00",
        "#00ff00",
        "#0000ff",
      ],
      stroke: {
        width: [4, 4, 2, 2, 1, 1],
        curve: [
          "smooth",
          "smooth",
          "stepline",
          "stepline",
          "smooth",
          "stepline",
        ],
        dashArray: [0, 0, 2, 2, 0, 0],
      },
      fill: {
        type: ["gradient", "gradient", "solid", "solid", "solid", "solid"],
        opacity: [0.35, 0.35, 0, 0, 0.1, 0.1],
      },
      legend: {
        show: true,
        showForNullSeries: false,
        labels: {
          useSeriesColors: true,
        },
      },
      markers: {
        size: 0,
      },
      chart: {
        animations: {
          enabled: false,
        },
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
            year: "yyyy",
            month: "MMM 'yy",
            day: "dd MMM",
            hour: "h:mm TT",
            minute: "h:mm:ss TT",
            second: "h:mm:ss TT",
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
            formatter: (v) => {
              return Math.floor(v);
            },
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
            formatter: (v) => {
              return Math.floor(v);
            },
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
        {
          max: 100,
          min: 0,
          show: false,
        },
        {
          max: 10,
          min: 0,
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
          formatter: (value) => {
            return parseFloat(value.toFixed(1));
          },
        },
      },
    },
  }),
};
</script>