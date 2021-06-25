<template>
<div>
    <div class="title"> Minisql </div>
    <div class="sqlarea">
        <el-input
        type="textarea"
        :rows="4"
        placeholder="请输入SQL语句"
        v-model="sqltext">
        </el-input>

    </div>
    <div class="btnarea">
        <el-button type="primary" @click="HandleExecute">Execute</el-button>
        <el-button type="primary" @click="HandleClear">清空结果</el-button>
    </div>
    <div class="ResultArea">
        <div> 结果 </div>
        <div>
            <el-input
            type="textarea"
            :rows="15"
            placeholder="结果区"
            v-model="consoletext"
            :disabled="true">
            </el-input>
        </div>

    </div>

</div>
</template>

<script>
import axios from 'axios'
export default {
    data(){
        return {
            sqltext: '',
            consoletext:''
            
        }
    },
    methods:{
        HandleExecute(){
            var query = this.sqltext.replace(/[\r\n]/g,"");   
            this.consoletext = 'running...';     
            axios.get('http://127.0.0.1:15000/api/minisql?sql=' + query)
            .then((response)=>{
                console.log(response);
                this.consoletext = response.data;
                while (this.consoletext.indexOf("\\n") >= 0) { this.consoletext = this.consoletext.replace("\\n", " \\n "); } 
                console.log(this.consoletext);
                // if(response.data.status == 100){
                //     sessionStorage.setItem('usr_id', response.data.usr_name);
                //     sessionStorage.setItem('pwd_session', response.data.pwd_session);
                //     sessionStorage.setItem('usr_access', response.data.access);
                //     this.$message("登陆成功")
                //     this.$router.go(-1);//返回上一层
                // }else if(response.data.status == 210){
                //     this.$message("用户名错误");
                // }else if(response.data.status == 220){
                //     this.$message("密码错误");
                // }else if(response.data.status == 310){
                //     this.$message("数据库操作错误："+ response.data.info);
                // }else{
                //     this.$message("未知的登录错误" + response.data.info)
                // }
            })
            .catch(function (error) {
                console.log(error);
            });
            console.log("Handle Execute");
        },
        HandleClear(){
            this.consoletext = '';
            console.log("Handle Clear")
        }
    },
    
}
</script>

<style scoped>
.title{
    margin:auto;
    font-size: 20px;
    padding-top: 5%;
    padding-bottom: 5%;
}
.sqlarea{
    margin:auto;
    width:80%;
}
.btnarea{
    margin-top: 20px;
}
.ResultArea{
    margin:auto;
    width:80%;
    padding: 5% 5% 5% 5%;
}
</style>