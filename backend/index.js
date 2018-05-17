const { spawn } = require('child_process');
const path = require('path');

const app = require('express')();
const cors = require('cors');
app.use(cors({
  origin: 'http://localhost:3000',
  credentials: true,
  optionsSuccessStatus: 204
}));

const server = require('http').Server(app);
const io = require('socket.io')(server);
app.io = io;
io.sockets.on('connection', (socket) => {
  console.log('socket connected');
  socket.emit('connected');
  socket.on('run', async (params) => {
    console.log(`Running algorithm with params ${JSON.stringify(params)}`);
    let algo = spawn('main', [params.func, params.D], {
      cwd: path.resolve(__dirname, 'algorithm')
    });
    let trailing = '';
    algo.stdout.on('data', (d) => {
      const alldata = (trailing + d.toString()).split('\n');
      alldata.forEach((data, i) => {
        if (data.length === 0)
          return;
        try {
          const json = JSON.parse(data);
          socket.emit(json.type, json.data);
        } catch (err) {
          // if (i === alldata.length - 1)
            trailing = data;
          // else
          //   console.log(`Parse data error, reason: ${err}, data: ${data}`);
        }
      });
    });
    algo.on('error', (err) => {
      console.log(`Run algorithm error, reason: ${err}`);
    });
    algo.on('exit', (code) => {
      console.log(`Algorithm finished with exit code ${code}`);
    });
  });
});

app.get('/', (req, res) => {
  res.send('ia backend working');
});

const PORT = 5000;
server.listen(PORT, () => {
  console.log(`Listening ${PORT}`);
});