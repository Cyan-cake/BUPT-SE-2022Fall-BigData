/*
 * 设置国际化的ajax请求header特定的Accept-Language
 */
(function (send) {
    XMLHttpRequest.prototype.send = function (data) {
        try {
            if (window.currentLanguage) {
                this.setRequestHeader('Accept-Language', window.currentLanguage);
            }
        } catch (error) {
            console.log('国际化设置Accept-Language异常' + error);
        }
        send.call(this, data);
    };
})(XMLHttpRequest.prototype.send);

/*
 * 设置国际化的页面跳转链接
 */
(function () {
    /*
    https://www.icourse163.org/course/CC-1002725002
    https://www.icourse163.org/user/setting/personInfoEdit.htm#/setting
    https://www.icourse163.org/university/CC#/c
    https://www.icourse163.org/learn/CC-1002725002?tid=1002811002#/learn/announce
    https://www.icourse163.org/help/help.htm#/hf?t=0
    http://www.icourse163.org/en/mooc/mob/course/richTextView.htm?tid=1003345002&uid=1004068009&cid=1003263002&lid=1003809006
    http://www.icourse163.org/en/mooc/mob/course/pdfView.htm?tid=1001617002&uid=1002172820&cid=1001541001&lid=1002072144
    */

    // 是否是国际化的链接
    function isIntlUrl(pathName) {
        return (
            pathName.match(/\/en\/mooc/) ||
            pathName.match(/\/mooc\/en/) ||
            pathName.indexOf('icourse163.com') > -1
        );
    }

    // 是否在国际化的翻译页面中
    function isInIntlUrl(pathName) {
        var courseReg = /^\/course\/\w*-\d*$/, // 课程页
            userSettingReg = /^\/user\/setting\/personInfoEdit\.htm$/, // 个人设置页
            universityReg = /^\/\university\/\w*$/, // 学校主页
            helpReg = /^\/\help\/help\.htm$/, // 帮助页
            learnReg = /^\/learn\/\w*-\d*$/, // 学习页
            richTextViewReg = /^\/mob\/course\/richTextView\.htm$/, // wap富文本课件页
            pdfViewReg = /^\/mob\/course\/pdfView\.htm$/; // wap pdf课件页
        var regs = [
            courseReg,
            userSettingReg,
            universityReg,
            helpReg,
            learnReg,
            richTextViewReg,
            pdfViewReg,
        ];
        for (var i = 0; i < regs.length; i++) {
            if (pathName.match(regs[i])) {
                return true;
            }
        }
    }

    function isImoocIntelUrl(pathName) {
        return pathName.match(/\/imooc/);
    }

    // 是否在新版的/en/imooc/路径底下
    function isInImoocUrl(pathName) {
        var schollCloudIndex = /^\/spoc\/schoolcloud\/index.htm$/, //学校云主页
            spocCourseReg = /^\/spoc\/course\/\w*-\d*$/, // 学校云课程页
            userSettingReg = /^\/user\/setting\/personInfoEdit\.htm$/, // 个人设置页
            spocUniversityReg = /^\/spoc\/university\/\w*$/, // spoc学校主页
            helpReg = /^\/\help\/help\.htm$/, // 帮助页
            spocLearnReg = /^\/spoc\/learn\/\w*-\d*$/, // spoc学习页
            liveroomReg = /^\/live\/liveRoom\.htm$/, //直播课
            teacherMainReg = /^\/u\/\w*$/, //老师主页
            homeReg = /^\/home\.htm$/, //个人中心
            coursePreview = /^\/course\/preview\/\w*-\d*$/, // 课程页面预览
            learnPreview = /^\/learn\/preview\/\w*-\d*$/, // 学习页预览
            university = /^\/university\/\w*$/; // 学校主页
        var regs = [
            schollCloudIndex,
            spocCourseReg,
            userSettingReg,
            spocUniversityReg,
            helpReg,
            spocLearnReg,
            liveroomReg,
            teacherMainReg,
            homeReg,
            coursePreview,
            learnPreview,
            university
        ];
        for (var i = 0; i < regs.length; i++) {
            if (pathName.match(regs[i])) {
                return true;
            }
        }
    }

    // 监听a标签的点击 暂时不加
    // document.addEventListener('click', function (e) {

    // });
    // 如果referrer是国际化的页面，当前url在这几个url内，跳到/en/mooc的链接（这里是一个兜底的跳转判断）
    if (isIntlUrl(document.referrer) && isInIntlUrl(location.pathname)) {
        location.pathname = '/en/mooc' + location.pathname; // 这里跳转到/en/mooc的链接
        return;
    } else if (
        isImoocIntelUrl(document.referrer) &&
        isInImoocUrl(location.pathname)
    ) {
        location.pathname = document.referrer.match(/\/(\w+)\/imooc/)[0] + location.pathname; // 这里跳转到/en/imooc的链接
        return;
    }
})();
