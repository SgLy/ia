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

import { Bar } from 'react-chartjs-2';

import io from 'socket.io-client';

import Canvas from './canvas.jsx';

import './App.css';
/* eslint-enable no-unused-vars */

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      func: 1,
      D: 2,
      map: [[0]],
      rowDigest: 0,
      best: [],
      distrib: [],
    };

    this.socket = io.connect('http://localhost:5000/');
    this.socket.on('connected', () => {
      // eslint-disable-next-line no-console
      console.log('socket connected');
    });
    this.socket.on('map', (map) => {
      this.setState({ map });
    });
    this.socket.on('round', (round) => {
      setTimeout(() => {
        const best = this.state.best;
        best.push({
          label: round.round,
          value: round.best
        });
        if (best.length > 20)
          best.shift();
        this.setState({ best });
      }, 0);
    });
    this.socket.on('best', (best) => {
      console.log(best);
    });
    this.socket.on('particles', (best) => {
      console.log(best);
    });
  }

  run = () => {
    // eslint-disable-next-line no-console
    console.log(`Request function ${this.state.func} with ${this.state.D} dim`);
    this.setState({
      map: [[]],
      best: []
    });
    this.socket.emit('run', { func: this.state.func, D: this.state.D });
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
              Intelligent Algorithm
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
          >
            Run
          </Button>
        </Paper>
        <div id="display">
          {this.state.D === 2 ? <Paper id="map">
            <Canvas map={this.state.map}></Canvas>
          </Paper> : ''}
          <Paper id='chart_best'>
            <Bar
              key={md5(this.state.best)}
              options={{
                legend: { display: false },
                scales: {
                  // xAxes: [{ barPercentage: 1, categoryPercentage: 1 }],
                  // yAxes: [{ ticks: { beginAtZero: true } }]
                },
                animation: {
                  duration: 0
                },
              }}
              data={{
                labels: this.state.best.map(r => r.label),
                datasets: [{
                  data: this.state.best.map(r => r.value),
                  backgroundColor: '#123456',
                  borderWidth: 0
                }]
              }}
            />
          </Paper>
          <Paper id='chart_distrib'>
            <Bar
              key={md5(this.state.distrib)}
              options={{
                legend: { display: false },
                scales: {
                  // xAxes: [{ barPercentage: 1, categoryPercentage: 1 }],
                  yAxes: [{ ticks: { beginAtZero: true } }]
                },
                animation: {
                  duration: 0
                },
              }}
              data={{
                labels: Array(this.state.distrib.length).fill(0).map((_, i) => i),
                datasets: [{
                  data: this.state.distrib,
                  backgroundColor: '#123456',
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
