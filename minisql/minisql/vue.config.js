const path = require('path')
function resolve (dir) {
  // 此处使用path.resolve 或path.join 可自行调整
  return path.join(__dirname, dir)
}

module.exports = {
  lintOnSave: false,
  // webpack 配置进行更细粒度的修改  https://cli.vuejs.org/zh/config/#chainwebpack
  chainWebpack: (config) => {
    // 修改文件引入自定义路径
    config.resolve.alias
      .set('@', resolve('src'))
  }
}
