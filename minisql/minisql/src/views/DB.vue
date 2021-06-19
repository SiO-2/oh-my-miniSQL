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
        <el-button type="primary" @click="HandleClear">清空</el-button>
    </div>
    <div class="ResultArea">
        <div> 结果 </div>
        <div>
            {{consoletext}}
        </div>

    </div>

</div>
</template>

<script>
export default {
    data(){
        return {
            sqltext: '',
            consoletext:''
            
        }
    },
    methods:{
        HandleExecute(){
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
    padding: 5% 5% 5% 5%;
}
</style>