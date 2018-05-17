import React, { Component } from 'react';
import data from './data.js';

class Canvas extends Component {
  constructor(props) {
    super(props);
    const data = props.data;
    this.id = `canvas${parseInt(Math.random() * 1e16, 10).toString(16)}`;
    const miny = Math.min(...data.map(d => Math.min(...d)));
    const maxy = Math.max(...data.map(d => Math.max(...d)));
    console.log(miny, maxy);
    this.colorMap = (v) => {
      let c = 255 * (v - miny) / (maxy - miny);
      return `rgb(${c}, 0, ${256 - c})`;
    };
  }

  componentDidMount = () => {
    const ctx = document.getElementById(this.id).getContext('2d');
    this.props.data.forEach((r, i) => {
      r.forEach((c, j) => {
        ctx.fillStyle = this.colorMap(c);
        ctx.fillRect(i / this.props.data.length * this.props.width, j / r.length * this.props.height, 1, 1);
      });
    });
  }

  render() {
    return (
      <canvas width={this.props.width} height={this.props.height} id={this.id}></canvas>
    )
  }
}

class App extends Component {
  render() {
    return (
      <Canvas
        width={400}
        height={400}
        data={data}
      ></Canvas>
      // <div>{colorMap(data[12][34])}</div>
    );
  }
}

export default App;
