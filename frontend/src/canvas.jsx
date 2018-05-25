/* eslint-disable no-unused-vars */
import React, { Component } from 'react';
/* eslint-enable no-unused-vars */

export default class Canvas extends Component {
  constructor(props) {
    super(props);
    this.id = `canvas${parseInt(Math.random() * 1e16, 10).toString(16)}`;
    this.state = { map: props.map };
    // this.height = this.state.map.length;
    // this.width = this.state.map[0].length;
    this.height = this.width = 401;
  }

  componentDidMount = () => {
    this.ctx = document.getElementById(this.id).getContext('2d');
    this.ctx.globalAlpha = 1;
    this.ctx.fillStyle = '#eee';
    this.ctx.fillRect(0, 0, this.props.width, this.props.height);
  }

  componentWillReceiveProps = (nextProps) => {
    if (this.state.map.length === nextProps.map.length)
      return;
    this.setState({ map: nextProps.map }, () => {
      // this.height = this.state.map.length;
      // this.width = this.state.map[0].length;
      setTimeout(() => {
        this.redraw();
      }, 0);
    });
  }

  redraw = () => {
    // eslint-disable-next-line no-console
    console.time('draw');
    const max = Math.max(...this.state.map.map(r => Math.max(...r)));
    const min = Math.min(...this.state.map.map(r => Math.min(...r)));
    this.colorMap = (v) => {
      let t = Math.sqrt((v - min) / (max - min));
      let c = Math.round(255 * t);
      return [c, 255 - c, 0, 255];
    };
    // window.requestAnimationFrame((time) => {
    //   this.redrawRow(time, 0);
    // });
    let imageData = this.ctx.getImageData(0, 0, this.width, this.height);
    let data = imageData.data, i = 0;
    this.state.map.forEach(row => {
      row.forEach(value => {
        [data[i], data[i + 1], data[i + 2], data[i + 3]] = this.colorMap(value);
        i += 4;
      });
    });
    this.ctx.putImageData(imageData, 0, 0);
    // eslint-disable-next-line no-console
    console.timeEnd('draw');
  }

  render() {
    return (
      <div className="canvas" style={{ position: 'relative' }}>
        <canvas
          width={this.width}
          height={this.height}
          id={this.id}
        ></canvas>
        <div className="particles" style={{
          position: 'absolute',
          height: this.height,
          width: this.width,
          top: 0,
          left: 0,
          backgroundColor: 'rgba(0, 0, 0, 0)'
        }}>
          {this.props.particles.map((p, i) => (
            <div className="particle" style={{
              position: 'absolute',
              height: 5,
              width: 5,
              borderRadius: '50%',
              top: (100 - p[1]) / 200 * this.height - 2.5,
              left: (p[0] + 100) / 200 * this.width - 2.5,
              backgroundColor: 'blue',
              transition: 'all 0.5s linear'
            }} key={this.id + i}></div>
          ))}
        </div>
      </div>
    );
  }
}