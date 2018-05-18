/* eslint-disable no-unused-vars */
import React, { Component } from 'react';
/* eslint-enable no-unused-vars */

export default class Canvas extends Component {
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
      // eslint-disable-next-line no-console
      console.log(`Draw row ${row.index} at ${time}`);
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