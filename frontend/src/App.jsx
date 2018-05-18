/* eslint-disable no-unused-vars */
import React, { Component } from 'react';

import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import Paper from '@material-ui/core/Paper';
import MenuItem from '@material-ui/core/MenuItem';
import Select from '@material-ui/core/Select';
import Button from '@material-ui/core/Button';
import FormControl from '@material-ui/core/FormControl';

import io from 'socket.io-client';

import Canvas from './canvas.jsx';

import './App.css';
import { InputLabel } from '@material-ui/core';
/* eslint-enable no-unused-vars */

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      func: 1,
      D: 2,
      mapLength: 0,
      mapValueRange: { min: 0, max: 0 }
    };

    this.rows = [];

    this.socket = io.connect('http://localhost:5000/');
    this.socket.on('connected', () => {
      // eslint-disable-next-line no-console
      console.log('socket connected');
    });
    this.socket.on('mapRowCount', (mapLength) => {
      this.setState({ mapLength });
    });
    this.socket.on('mapRow', (mapRow) => {
      this.rows.push(mapRow);
    });
    this.socket.on('mapValueRange', (mapValueRange) => {
      this.setState({ mapValueRange });
    });
  }

  digest = () => {
    return this.rows.length === 0 ? undefined : this.rows.shift();
  }

  run = () => {
    console.log({ func: this.state.func, D: this.state.D });
    this.setState({
      mapLength: 0,
      mapValueRange: { max: 0, min: 0 }
    });
    this.rows = [];
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
          <Paper id="map">
            {this.state.D === 2 ? <Canvas
              width={400}
              height={400}
              mapLength={this.state.mapLength}
              valueRange={this.state.mapValueRange}
              requestNewRow={this.digest}
            ></Canvas> : ''}
          </Paper>
        </div>
      </div>
    );
  }
}

export default App;
