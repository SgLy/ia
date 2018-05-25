/* eslint-disable no-unused-vars */
import React, { Component } from 'react';

import {
  AppBar, Toolbar, Typography,
  Paper,
  Select, MenuItem,
  Button,
  FormControl, InputLabel,
} from '@material-ui/core';

import md5 from 'md5';
import _ from 'lodash';

import { Bar, Line } from 'react-chartjs-2';

import io from 'socket.io-client';

import Canvas from './canvas.jsx';

import './App.css';
/* eslint-enable no-unused-vars */

function partition(v, p) {
  let result = { distrib: [], label: [] };
  let start = 0, cnt = 1;
  for (let i = 1; i < v.length; ++i) {
    if (v[i] - v[start] < p)
      ++cnt;
    else {
      result.distrib.push(cnt);
      result.label.push(v[start].toFixed(2));
      cnt = 1;
      start = i;
    }
  }
  if (start !== v.length - 1) {
    result.distrib.push(cnt);
    result.label.push(v[start].toFixed(2));
  }
  return result;
}

function bestPartition(v, n) {
  v.sort((a, b) => a - b);
  let left = 1e-4, right = v[v.length - 1] - v[0];
  while (right - left > 1e-4) {
    const mid = (left + right) / 2;
    const { distrib } = partition(v, mid);
    if (distrib.length > n)
      left = mid;
    else
      right = mid;
  }
  return partition(v, left);
}

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      func: 1,
      D: 2,
      map: [[]],
      particles: [],
      rowDigest: 0,
      best: { PSO: [], DE: [] },
      distribP: [], labelP: [],
      distribD: [], labelD: [],
      connected: false,
      running: false
    };

    this.socket = io.connect('http://172.18.159.6:5000/');
    this.socket.on('connected', () => {
      // eslint-disable-next-line no-console
      console.log('socket connected');
      this.setState({ connected: true });
    });
    this.socket.on('map', (map) => {
      this.setState({ map });
    });
    this.particles = [];
    this.group = [];
    this.PSO = [];
    this.DE = [];
    this.socket.on('particles', (particles) => {
      this.particles.push(particles);
      this.refresh();
    });
    this.socket.on('PSO', PSO => {
      this.PSO.push(PSO);
      this.refresh();
    });
    this.socket.on('DE', DE => {
      this.DE.push(DE);
      this.refresh();
    });
    this.socket.on('group', group => {
      this.group.push(group);
      this.refresh();
    });
    this.socket.on('done', () => {
      this.setState({ running: false });
    });
  }

  refresh = _.throttle(() => {
    const best = this.state.best;
    ['PSO', 'DE'].forEach(type => {
      if (this[type].length === 0)
        return;
      const round = this[type].shift();
      best[type].push({
        label: round.round,
        value: round.best
      });
      if (best[type].length >= 2 && best[type][best[type].length - 1].value > best[type][best[type].length - 2]) {
        best[type][best[type].length - 1].value = best[type][best[type].length - 2].value;
      }
      // if (best[type].length > 10)
      //   best[type].shift();
    });
    if (this.particles.length > 0) {
      const particles = this.particles.shift();
      let v = particles.map(p => p.value);
      const {distrib, label} = bestPartition(v, 7);
      this.setState({
        particles,
        distribP: distrib,
        labelP: label
      });
    }
    if (this.group.length > 0) {
      const group = this.group.shift();
      const { distrib, label } = bestPartition(group, 7);
      this.setState({
        distribD: distrib,
        labelD: label
      });
    }
    this.setState({ best }, this.refresh);
  }, 500)

  run = () => {
    // eslint-disable-next-line no-console
    console.log(`Request function ${this.state.func} with ${this.state.D} dim`);
    this.setState({
      map: [[]],
      best: { PSO: [], DE: [] },
      running: true,
      distribP: [], labelP: [],
      distribD: [], labelD: []
    });
    this.PSO = [];
    this.DE = [];
    this.socket.emit('run', { func: this.state.func, D: this.state.D });
  }

  stop = () => {
    this.socket.emit('stop');
    this.refresh.cancel();
  }

  handleChange = (e) => {
    this.setState({
      [e.target.name]: parseInt(e.target.value, 10)
    });
  }

  render() {
    return (
      <div className="root">
        <AppBar position="static" color="default">
          <Toolbar>
            <Typography variant="title" color="inherit">
              {this.state.connected ? 'Intelligent Algorithm' : 'Connecting...'}
            </Typography>
          </Toolbar>
        </AppBar>
        <Paper className="control">
          <FormControl id='select_func'>
            <InputLabel>Function</InputLabel>
            <Select
              value={this.state.func}
              onChange={this.handleChange}
              inputProps={{
                name: 'func'
              }}
            >
              <MenuItem value={1}>
                Bent Cigar Function
              </MenuItem>
              <MenuItem value={2}>
                Sum of Different Power Function
              </MenuItem>
              <MenuItem value={3}>
                Zakharov Function
              </MenuItem>
              <MenuItem value={4}>
                Shifted and Rotated Rosenbrock’s Function
              </MenuItem>
              <MenuItem value={5}>
                Shifted and Rotated Rastrigin’s Function
              </MenuItem>
              <MenuItem value={6}>
                Shifted and Rotated Schaffer’s F7 Function
              </MenuItem>
              <MenuItem value={7}>
                Shifted and Rotated Lunacek Bi-Rastrigin's Function
              </MenuItem>
            </Select>
          </FormControl>
          <FormControl id='select_d'>
            <InputLabel>Dimensions</InputLabel>
            <Select
              value={this.state.D}
              onChange={this.handleChange}
              id='select_d'
              inputProps={{
                id: 'select_d',
                name: 'D'
              }}
            >
              <MenuItem value={2}>2</MenuItem>
              <MenuItem value={10}>10</MenuItem>
              <MenuItem value={30}>30</MenuItem>
              <MenuItem value={50}>50</MenuItem>
              <MenuItem value={100}>100</MenuItem>
            </Select>
          </FormControl>
          <Button
            variant='flat'
            color='primary'
            id='button_run'
            onClick={this.run}
            disabled={this.state.running}
          >
            {this.state.running ? 'Running...' : 'Run'}
          </Button>
          {!this.state.running ? <Button
            variant='flat'
            color='secondary'
            id='button_stop'
            onClick={this.stop}
          >
            Stop animation
          </Button> : ''}
        </Paper>
        <div id="display">
          {this.state.D === 2 ? <Paper id="map">
            <Canvas
              map={this.state.map}
              particles={this.state.particles.map(p => p.position)}
            ></Canvas>
          </Paper> : ''}
          <Paper id='chart_best'>
            <Line
              key={md5(this.state.best)}
              height={400}
              options={{
                maintainAspectRatio: false,
                // legend: { display: false },
                scales: {
                  // yAxes: [{ ticks: { beginAtZero: true } }]
                },
                animation: {
                  duration: 0
                },
              }}
              data={{
                labels: Array(this.state.best.PSO.length).fill(0).map((_, i) => i + 1),
                datasets: [{
                  data: this.state.best.PSO.map(r => r.value),
                  borderColor: '#123456',
                  fill: false,
                  label: 'PSO',
                  lineTension: 0
                }, {
                  data: this.state.best.DE.map(r => r.value),
                  borderColor: '#345612',
                  fill: false,
                  label: 'DE',
                  lineTension: 0
                }]
              }}
            />
          </Paper>
          <Paper id='chart_distrib'>
            <Bar
              // key={md5(this.state.distribP)}
              // width={400}
              height={400}
              options={{
                maintainAspectRatio: false,
                legend: { display: false },
                scales: {
                  xAxes: [{
                    ticks: {
                      autoSkip: false
                    }
                  }],
                  yAxes: [{ ticks: { beginAtZero: true } }]
                },
                animation: {
                  duration: 0
                },
              }}
              data={{
                labels: Array(
                  Math.max(this.state.labelP.length, this.state.labelD.length)
                )
                  .fill(0)
                  .map((_, i) => [this.state.labelP[i], this.state.labelD[i] || '']),
                datasets: [{
                  data: this.state.distribP,
                  backgroundColor: '#123456',
                  label: 'PSO',
                  borderWidth: 0
                }, {
                  data: this.state.distribD,
                  backgroundColor: '#345612',
                  label: 'DE',
                  borderWidth: 0
                }]
              }}
            />
          </Paper>
        </div>
      </div>
    );
  }
}

export default App;
