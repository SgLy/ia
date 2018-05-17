const { execFile } = require('mz/child_process');
const { spawn } = require('child_process');
const { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');
const osType = require('os').type();
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
    let algo = spawn('main', [], {
      cwd: path.resolve(__dirname, 'algorithm')
    });
    algo.stdout.on('data', (d) => {
      d.toString().split('\n').forEach(data => {
        if (data.length === 0)
          return;
        try {
          data = JSON.parse(data);
        } catch (err) {
          console.log(`Parse data error, reason: ${err}, data: ${data}`);
        }
        socket.emit('data', data);
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