import React, { Component } from 'react';
import io from 'socket.io-client';
import data from './data.json';

class Canvas extends Component {
  constructor(props) {
    super(props);
    const data = props.data;
    this.id = `canvas${parseInt(Math.random() * 1e16, 10).toString(16)}`;
    const miny = Math.min(...data.map(d => Math.min(...d)));
    const maxy = Math.max(...data.map(d => Math.max(...d)));
    this.colorMap = (v) => {
      let t = Math.sqrt((v - miny) / (maxy - miny));
      let c = 255 * t;
      return `rgb(${c}, ${255 - c}, 0)`;
    };
  }

  componentDidMount = () => {
    const ctx = document.getElementById(this.id).getContext('2d');
    const w = this.props.width / this.props.data.length;
    this.props.data.forEach((r, i) => {
      const h = this.props.height / r.length;
      r.forEach((c, j) => {
        ctx.fillStyle = this.colorMap(c);
        ctx.fillRect(i * w, j * h, w, h);
      });
    });

    const socket = io.connect('http://localhost:5000/');
    socket.on('connected', (msg) => {
      console.log('socket connected');
      socket.emit('run');
    });
    socket.on('data', (msg) => {
      console.log(msg);
    });
  }

  render() {
    return (
      <canvas
        width={this.props.width}
        height={this.props.height}
        id={this.id}
      ></canvas>
    )
  }
}

class App extends Component {
  render() {
    return (
      <div>
        <Canvas
          width={400}
          height={400}
          data={data}
        ></Canvas>
      </div>
    );
  }
}

export default App;
