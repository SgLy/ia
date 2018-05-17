/* eslint-disable no-unused-vars */
import React, { Component } from 'react';
import io from 'socket.io-client';
/* eslint-enable no-unused-vars */


// eslint-disable-next-line no-unused-vars
class Select extends Component {
  constructor(props) {
    super(props);
    this.state = { value: 1 };
  }

  change = (e) => {
    this.setState({ value: e.target.value });
    this.props.onChange(e.target.value);
  }

  render() {
    return (
      <select onChange={this.change} value={this.state.value}>
        {this.props.options.map((option, i) => (
          <option key={i} value={option.value}>
            {option.text}
          </option>
        ))}
      </select>
    );
  }
}

// eslint-disable-next-line no-unused-vars
class Canvas extends Component {
  constructor(props) {
    super(props);
    this.id = `canvas${parseInt(Math.random() * 1e16, 10).toString(16)}`;
    this.state = {
      mapLength: 0,
      range: { min: 0, max: 0 }
    };
    this.colorMap = () => '#eee';
  }

  componentDidMount = () => {
    this.ctx = document.getElementById(this.id).getContext('2d');
    this.ctx.globalAlpha = 1;
    this.redraw();
  }

  componentWillReceiveProps = (nextProps) => {
    this.setRange(nextProps.valueRange);
    if (nextProps.mapLength !== this.state.mapLength) {
      this.setState({
        mapLength: nextProps.mapLength
      }, () => {
        this.redraw();
      });
      this.w = Math.round(this.props.width / nextProps.mapLength);
    }
  }

  setRange = (range) => {
    if (range.min === this.state.min && range.max === this.state.max)
      return;
    this.colorMap = (v) => {
      let t = Math.sqrt((v - range.min) / (range.max - range.min));
      let c = Math.round(255 * t);
      return `rgb(${c}, ${255 - c}, 0)`;
    };
    this.setState(range);
  }

  redraw = () => {
    this.ctx.fillStyle = '#eee';
    this.ctx.fillRect(0, 0, this.props.width, this.props.height);
    if (this.state.mapLength === 0)
      return;
    this.drawCount = 0;
    window.requestAnimationFrame((time) => {
      this.redrawRow(time);
    });
  }

  redrawRow = (time, row) => {
    if (row !== undefined) {
      ++this.drawCount;
      console.log(`draw row ${row.index} at ${time}`);
      const left = Math.round(row.index * this.w);
      let top = 0;
      const h = Math.round(this.props.height / row.values.length);
      row.values.forEach(c => {
        this.ctx.fillStyle = this.colorMap(c);
        this.ctx.fillRect(left, top += h, this.w, h);
      });
    }
    if (this.drawCount < this.state.mapLength)
      window.requestAnimationFrame((time) => {
        this.redrawRow(time, this.props.requestNewRow());
      });
  }

  render() {
    return (
      <canvas
        width={this.props.width}
        height={this.props.height}
        id={this.id}
      ></canvas>
    );
  }
}

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

  changeFunc = (func) => {
    this.setState({ func: parseInt(func, 10) })
  }

  changeD = (D) => {
    this.setState({ D: parseInt(D, 10) });
  }

  render() {
    return (
      <div>
        {this.state.D === 2 ? <Canvas
          width={400}
          height={400}
          mapLength={this.state.mapLength}
          valueRange={this.state.mapValueRange}
          requestNewRow={this.digest}
        ></Canvas> : ''}
        <Select
          onChange={this.changeFunc}
          name="func"
          options={[
            { value: 1, text: 'Bent Cigar Function' },
            { value: 2, text: 'Sum of Different Power Function' },
            { value: 3, text: 'Zakharov Function' },
            { value: 4, text: 'Shifted and Rotated Rosenbrock’s Function' },
            { value: 5, text: 'Shifted and Rotated Rastrigin’s Function' },
            { value: 6, text: 'Shifted and Rotated Schaffer’s F7 Function' },
            { value: 7, text: 'Shifted and Rotated Lunacek Bi-Rastrigin\'s Function' },
          ]}
        ></Select>
        <Select
          onChange={this.changeD}
          options={[
            { value: 2, text: '2' },
            { value: 10, text: '10' },
            { value: 30, text: '30' },
            { value: 50, text: '50' },
            { value: 100, text: '100' },
          ]}
        ></Select>
        <button onClick={this.run}>Run</button>
      </div>
    );
  }
}

export default App;
