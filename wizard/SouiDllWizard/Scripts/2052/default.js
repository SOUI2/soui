
function OnFinish(selProj, selObj) {
    try {
        var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
        var strProjectName = wizard.FindSymbol('PROJECT_NAME');

        selProj = CreateCustomProject(strProjectName, strProjectPath);
        AddConfig(selProj, strProjectName);
        AddFilters(selProj);

        var InfFile = CreateCustomInfFile();
        AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);
        PchSettings(selProj);
        InfFile.Delete();

        selProj.Object.Save();
    }
    catch (e) {
        if (e.description.length != 0)
            SetErrorInfo(e);
        return e.number
    }
}

function CreateCustomProject(strProjectName, strProjectPath) {
    try {
        var supportXp = wizard.FindSymbol('SUPPORT_XP');
        var strProjTemplatePath = '';
        var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');
        strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
        if (supportXp != 1 && WizardVersion > 10.0) {
            strProjTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
            strProjTemplatePath += '\\porjectTemplatesDefault';
            if (WizardVersion == 10.0)
                strProjTemplatePath += '\\2010';
            else if (WizardVersion == 11.0)
                strProjTemplatePath += '\\2012';
            else if (WizardVersion == 12.0)
                strProjTemplatePath += '\\2013';
			else if (WizardVersion == 14.0)
                strProjTemplatePath += '\\2015';
            else if (WizardVersion == 15.0)
                strProjTemplatePath += '\\2017';
        }
        if (supportXp == 1 && WizardVersion > 10.0) {
            strProjTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
            strProjTemplatePath += '\\porjectTemplates';

			if (WizardVersion == 10.0)
                strProjTemplatePath += '\\2010';
            else if (WizardVersion == 11.0)
                strProjTemplatePath += '\\2012';
            else if (WizardVersion == 12.0)
                strProjTemplatePath += '\\2013';
            else if (WizardVersion == 14.0)
                strProjTemplatePath += '\\2015';
            else if (WizardVersion == 15.0)
                strProjTemplatePath += '\\2017';
        }

        var strProjTemplate = '';

        if (WizardVersion >= 10.0)
            strProjTemplate = strProjTemplatePath + '\\default.vcxproj';
        else
            strProjTemplate = strProjTemplatePath + '\\default.vcproj';

        var Solution = dte.Solution;
        var strSolutionName = "";
        if (wizard.FindSymbol("CLOSE_SOLUTION")) {
            Solution.Close();
            strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
            if (strSolutionName.length) {
                var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
                Solution.Create(strSolutionPath, strSolutionName);
            }
        }

        var strProjectNameWithExt = '';
        if (WizardVersion >= 10.0)
            strProjectNameWithExt = strProjectName + '.vcxproj';
        else
            strProjectNameWithExt = strProjectName + '.vcproj';


        var oTarget = wizard.FindSymbol("TARGET");
        var prj;
        if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
        {
            var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
            prj = prjItem.SubProject;
        }
        else {
            prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
        }
        var fxtarget = wizard.FindSymbol("TARGET_FRAMEWORK_VERSION");
        if (fxtarget != null && fxtarget != "") {
            fxtarget = fxtarget.split('.', 2);
            if (fxtarget.length == 2)
                prj.Object.TargetFrameworkVersion = parseInt(fxtarget[0]) * 0x10000 + parseInt(fxtarget[1])
        }
        return prj;
    }
    catch (e) {
        throw e;
    }
}

function AddFilters(proj) {
    try {
        // 将文件夹添加到项目
        var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
        var group = proj.Object.AddFilter('Source Files');
        group.Filter = strSrcFilter;

        var strHeaderFilter = wizard.FindSymbol('HEADER_FILTER');
        var group = proj.Object.AddFilter('Header Files');
        group.Filter = strHeaderFilter;

        var strResFilter = wizard.FindSymbol('RESOURCE_FILTER');
        var group = proj.Object.AddFilter('Resource Files');
        group.Filter = strResFilter;

        var strSouiFilter = wizard.FindSymbol('SOUIRES_FILTER');
        var group = proj.Object.AddFilter('SoUI Resouece');
        group.Filter = strSouiFilter;
        if (wizard.FindSymbol('CHECKBOX_SYSRES_BUILTIN') && (wizard.FindSymbol('ResLoaderType') != 0)) {
            var group = proj.Object.AddFilter('SoUI Sys Resouece');
            group.Filter = strSouiFilter;
        }
    }
    catch (e) {
        throw e;
    }
}

function AddConfig(proj, strProjectName) {
    try {
        var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');
        var SysResBuiltin = wizard.FindSymbol('CHECKBOX_SYSRES_BUILTIN');
        var ResLoadType = wizard.FindSymbol('ResLoaderType');

        var unicodeSet = wizard.FindSymbol('UNICODE');
        var wcharSet = wizard.FindSymbol('WCHAR');
        var mtSet = wizard.FindSymbol('MT');
        // Debug设置
        var config = proj.Object.Configurations('Debug');
        config.CharacterSet = (unicodeSet == 1) ? charSetUNICODE : charSetMBCS;
        if (WizardVersion >= 10.0) {
            config.IntermediateDirectory = '$(Configuration)\\';
            config.OutputDirectory = '$(SolutionDir)$(Configuration)\\';
        }
        else {
            config.IntermediateDirectory = '$(ConfigurationName)';
            config.OutputDirectory = '$(SolutionDir)$(ConfigurationName)';
        }

        var CLTool = config.Tools('VCCLCompilerTool');
        //添加编译器设置
        CLTool.UsePrecompiledHeader = 2;    // 2-使用预编译头,1-创建,0-不使用
        CLTool.SuppressStartupBanner = true;
        CLTool.TreatWChar_tAsBuiltInType = (wcharSet == 1);
        CLTool.WarningLevel = warningLevelOption.warningLevel_3;
        CLTool.AdditionalIncludeDirectories = '"$(SOUIPATH)\\config";"$(SOUIPATH)\\components";"$(SOUIPATH)\\SOUI\\include";"$(SOUIPATH)\\utilities\\include"';
        CLTool.PreprocessorDefinitions = 'WIN32;_WINDOWS;_USRDLL;STRICT;_DEBUG';
        CLTool.RuntimeLibrary = (mtSet == 1) ? 1 : 3; // 0=MT, 1=MTd, 2=MD (DLL), 3=MDd
        CLTool.BrowseInformation = browseInfoOption.brAllInfo;// FR
        CLTool.Optimization = optimizeOption.optimizeDisabled;// Od
        CLTool.DebugInformationFormat = debugOption.debugEditAndContinue;//Edit and continue

        var LinkTool = config.Tools('VCLinkerTool');
        //添加链接器设置
        LinkTool.GenerateDebugInformation = true;
        LinkTool.LinkIncremental = linkIncrementalYes;
        LinkTool.SuppressStartupBanner = true;  // nologo
        LinkTool.GenerateDebugInformation = true;
        LinkTool.AdditionalLibraryDirectories = '"$(SOUIPATH)\\bin"';
        LinkTool.AdditionalDependencies = 'utilitiesd.lib souid.lib'
        LinkTool.SubSystem = subSystemOption.subSystemWindows;

        var resCplTool = config.Tools('VCResourceCompilerTool');
        resCplTool.Culture = 0x804;
        if (SysResBuiltin && (ResLoadType == 0)) {
            resCplTool.AdditionalIncludeDirectories = '"$(SOUIPATH)\\soui-sys-resource"';
        }
		else {
			resCplTool.PreprocessorDefinitions += ';_DEBUG';	
		}
        // Release设置
        var config = proj.Object.Configurations('Release');
        config.CharacterSet = (unicodeSet == 1) ? charSetUNICODE : charSetMBCS;
        if (WizardVersion >= 10.0) {
            config.IntermediateDirectory = '$(Configuration)\\';
            config.OutputDirectory = '$(SolutionDir)$(Configuration)\\';
        }
        else {
            config.IntermediateDirectory = '$(ConfigurationName)';
            config.OutputDirectory = '$(SolutionDir)$(ConfigurationName)';
        }
        var CLTool = config.Tools('VCCLCompilerTool');
        //添加编译器设置
        CLTool.UsePrecompiledHeader = 2;    // 2-使用预编译头,1-创建,0-不使用
        CLTool.SuppressStartupBanner = true;
        CLTool.TreatWChar_tAsBuiltInType = (wcharSet == 1);
        CLTool.WarningLevel = warningLevelOption.warningLevel_3;
        CLTool.AdditionalIncludeDirectories = '"$(SOUIPATH)\\config";"$(SOUIPATH)\\components";"$(SOUIPATH)\\SOUI\\include";"$(SOUIPATH)\\utilities\\include"';
        CLTool.PreprocessorDefinitions = 'WIN32;_WINDOWS;_USRDLL;NDEBUG';
        CLTool.RuntimeLibrary = (mtSet == 1) ? 0 : 2;; // 0=MT, 1=MTd, 2=MD (DLL), 3=MDd
        CLTool.WholeProgramOptimization = true;	//全程序优化：启动链接时代码生成

        var LinkTool = config.Tools('VCLinkerTool');
        //添加链接器设置
        LinkTool.GenerateDebugInformation = true;
        LinkTool.LinkIncremental = linkIncrementalYes;
        LinkTool.SuppressStartupBanner = true;  // nologoif(UserDll)
        LinkTool.AdditionalLibraryDirectories = '"$(SOUIPATH)\\bin"';
        LinkTool.AdditionalDependencies = 'utilities.lib soui.lib'
        LinkTool.LinkIncremental = 1;
        LinkTool.SubSystem = subSystemOption.subSystemWindows;

        var resCplTool = config.Tools('VCResourceCompilerTool');
        resCplTool.Culture = 0x804;
        if (SysResBuiltin && (ResLoadType == 0)) {
            resCplTool.AdditionalIncludeDirectories = '"$(SOUIPATH)\\soui-sys-resource"';
        }
        //x64配置,默认情况15以前的版本是没有X64的配置的
        var config_x64 = proj.Object.Configurations('Debug|x64');
        if (config_x64 != null) {
            config_x64.CharacterSet = (unicodeSet == 1) ? charSetUNICODE : charSetMBCS;
            if (WizardVersion >= 10.0) {
                config_x64.IntermediateDirectory = '$(Configuration)64\\';
                config_x64.OutputDirectory = '$(SolutionDir)$(Configuration)64\\';
            }
            else {
                config_x64.IntermediateDirectory = '$(ConfigurationName)64';
                config_x64.OutputDirectory = '$(SolutionDir)$(ConfigurationName)64';
            }
            var CLTool_x64 = config_x64.Tools('VCCLCompilerTool');
            //添加64位编译器设置
            CLTool_x64.UsePrecompiledHeader = 2;    // 2-使用预编译头,1-创建,0-不使用
            CLTool_x64.SuppressStartupBanner = true;
            CLTool_x64.TreatWChar_tAsBuiltInType = (wcharSet == 1);
            CLTool_x64.WarningLevel = warningLevelOption.warningLevel_3;
            CLTool_x64.AdditionalIncludeDirectories = '"$(SOUIPATH)\\config";"$(SOUIPATH)\\components";"$(SOUIPATH)\\SOUI\\include";"$(SOUIPATH)\\utilities\\include"';
            CLTool_x64.PreprocessorDefinitions = 'WIN64;_WINDOWS;_USRDLL;STRICT;_DEBUG';
            CLTool_x64.RuntimeLibrary = (mtSet == 1) ? 1 : 3; // 0=MT, 1=MTd, 2=MD (DLL), 3=MDd
            CLTool_x64.BrowseInformation = browseInfoOption.brAllInfo;// FR
            CLTool_x64.Optimization = optimizeOption.optimizeDisabled;// Od
            CLTool_x64.DebugInformationFormat = debugOption.debugEditAndContinue;//Edit and continue

            var LinkTool_64 = config_x64.Tools('VCLinkerTool');
            //添加链接器设置
            LinkTool_64.GenerateDebugInformation = true;
            LinkTool_64.LinkIncremental = linkIncrementalYes;
            LinkTool_64.SuppressStartupBanner = true;  // nologo
            LinkTool_64.GenerateDebugInformation = true;
            LinkTool_64.AdditionalLibraryDirectories = '"$(SOUIPATH)\\bin64"';
            LinkTool_64.AdditionalDependencies = 'utilitiesd.lib souid.lib'
            LinkTool_64.SubSystem = subSystemOption.subSystemWindows;
            var resCplTool_64 = config_x64.Tools('VCResourceCompilerTool');
            resCplTool_64.Culture = 0x804;
            resCplTool_64.PreprocessorDefinitions += ';_DEBUG';
            if (SysResBuiltin && (ResLoadType == 0)) {
                resCplTool_64.AdditionalIncludeDirectories = '"$(SOUIPATH)\\soui-sys-resource"';
            }
            var config_64 = proj.Object.Configurations('Release|x64');
            config_64.CharacterSet = (unicodeSet == 1) ? charSetUNICODE : charSetMBCS;
            if (WizardVersion >= 10.0) {
                config_64.IntermediateDirectory = '$(Configuration)64\\';
                config_64.OutputDirectory = '$(SolutionDir)$(Configuration)64\\';
            }
            else {
                config_64.IntermediateDirectory = '$(ConfigurationName)64';
                config_64.OutputDirectory = '$(SolutionDir)$(ConfigurationName)64';
            }
            var CLTool_x64 = config_64.Tools('VCCLCompilerTool');
            //添加编译器设置
            CLTool_x64.UsePrecompiledHeader = 2;    // 2-使用预编译头,1-创建,0-不使用
            CLTool_x64.SuppressStartupBanner = true;
            CLTool_x64.TreatWChar_tAsBuiltInType = (wcharSet == 1);
            CLTool_x64.WarningLevel = warningLevelOption.warningLevel_3;
            CLTool_x64.AdditionalIncludeDirectories = '"$(SOUIPATH)\\config";"$(SOUIPATH)\\components";"$(SOUIPATH)\\SOUI\\include";"$(SOUIPATH)\\utilities\\include"';
            CLTool_x64.PreprocessorDefinitions = 'WIN64;_WINDOWS;_USRDLL;NDEBUG';
            CLTool_x64.RuntimeLibrary = (mtSet == 1) ? 0 : 2;; // 0=MT, 1=MTd, 2=MD (DLL), 3=MDd
            CLTool_x64.WholeProgramOptimization = true;	//全程序优化：启动链接时代码生成
            var LinkTool_x64 = config_64.Tools('VCLinkerTool');
            //添加链接器设置
            LinkTool_x64.GenerateDebugInformation = true;
            LinkTool_x64.LinkIncremental = linkIncrementalYes;
            LinkTool_x64.SuppressStartupBanner = true;  // nologoif(UserDll)
            LinkTool_x64.AdditionalLibraryDirectories = '"$(SOUIPATH)\\bin64"';
            LinkTool_x64.AdditionalDependencies = 'utilities.lib soui.lib'
            LinkTool_x64.LinkIncremental = 1;
            LinkTool_x64.SubSystem = subSystemOption.subSystemWindows;

            var resCplTool_64 = config_64.Tools('VCResourceCompilerTool');
            resCplTool_64.Culture = 0x804;
            if (SysResBuiltin && (ResLoadType == 0)) {
                resCplTool_64.AdditionalIncludeDirectories = '"$(SOUIPATH)\\soui-sys-resource"';
            }
        }
    }
    catch (e) {
        alert(e.message);
        throw e;
    }
}

function PchSettings(proj) {
    // TODO: 指定 pch 设置
}

function DelFile(fso, strWizTempFile) {
    try {
        if (fso.FileExists(strWizTempFile)) {
            var tmpFile = fso.GetFile(strWizTempFile);
            tmpFile.Delete();
        }
    }
    catch (e) {
        throw e;
    }
}

function CreateCustomInfFile() {
    try {
        var fso, TemplatesFolder, TemplateFiles, strTemplate;
        fso = new ActiveXObject('Scripting.FileSystemObject');

        var TemporaryFolder = 2;
        var tfolder = fso.GetSpecialFolder(TemporaryFolder);
        var strTempFolder = tfolder.Drive + '\\' + tfolder.Name;

        var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
        var strInfFile = strTemplatePath + '\\Templates.inf';

        wizard.RenderTemplate(strInfFile, strWizTempFile);

        var WizTempFile = fso.GetFile(strWizTempFile);
        return WizTempFile;
    }
    catch (e) {
        throw e;
    }
}

function GetSourceName(strName) {
    try {
        strName.toLowerCase();
        if (strName.indexOf('[uires]') == 0) {
            strName = "uires\\" + strName.substr(7);
        }
        else if (strName.indexOf('[theme_sys_res]') == 0) {
            strName = "theme_sys_res\\" + strName.substr(15);
        }
        return strName;
    }
    catch (e) {
        throw e;
    }
}

function GetTargetName(strName, strProjectName) {
    try {
        strName.toLowerCase();
        // TODO: 基于模板文件名设置呈现文件的名称
        var strTarget = strName;

        if (strName == 'demo.cpp')
            strTarget = strProjectName + '.cpp';

        if (strName == 'demo.rc')
            strTarget = strProjectName + '.rc';

        if (strName.indexOf('[uires]') == 0) // UI资源文件
        {
            strTarget = "uires\\" + strName.substr(7);
        }
        if (strName.indexOf('[theme_sys_res]') == 0) {
            strTarget = "uires\\theme_sys_res\\" + strName.substr(15);
        }
        return strTarget;
    }
    catch (e) {
        throw e;
    }
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile) {
    try {
        var projItems = proj.ProjectItems

        var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

        var strTpl = '';
        var strName = '';

        // 过滤器对象
        var projFilters = proj.Object.Filters;
        var filterRes = projFilters.Item('Resource Files');
        var filterUIRES = projFilters.Item('SoUI Resouece');
        var filterSysRes = projFilters.Item('SoUI Sys Resouece');
        var strTextStream = InfFile.OpenAsTextStream(1, -2);
        while (!strTextStream.AtEndOfStream) {
            strTpl = strTextStream.ReadLine();
            if (strTpl != '' && strTpl.indexOf(';') != 0) // ;注释行
            {
                var bCopyOnly = false;  //“true”仅将文件从 strTemplate 复制到 strTarget，而不对项目进行呈现/添加
                var bBinary = false;
                if (strTpl.indexOf('=') == 0) // 二进制文件
                {
                    bBinary = true;
                    strTpl = strTpl.substr(1);
                }

                if (strTpl.indexOf('-') == 0) // 只拷贝不需要添加的文件
                {
                    bCopyOnly = true;
                    strTpl = strTpl.substr(1);
                }

                strName = strTpl;

                var strTarget = GetTargetName(strName, strProjectName);
                var strSource = GetSourceName(strName);

                var filter = null;
                if (strTpl.indexOf('[uires]') == 0) // soui res
                {
                    filter = filterUIRES;
                }
                if (strTpl.indexOf('[theme_sys_res]') == 0) {
                    filter = filterSysRes;
                }
                var strTemplate = strTemplatePath + '\\' + strSource;
                var strFile = strProjectPath + '\\' + strTarget;

                var strExt = strName.substr(strName.lastIndexOf("."));
                if (strExt == ".bmp" || strExt == ".ico" || strExt == ".gif" || strExt == ".rtf" || strExt == ".css" || strExt == ".png" || strExt == ".jpg" || strExt == ".lua")
                    bBinary = true;
                // 复制文件和添加到工程
                wizard.RenderTemplate(strTemplate, strFile, bBinary);
                if (!bCopyOnly) {
                    if (filter) {
                        filter.AddFile(strTarget);
                    } else {
                        proj.Object.AddFile(strFile);
                    }
                }
            }
        }
        strTextStream.Close();

        // 设置某些文件的编译选项
        var files = proj.Object.Files;
        var file = files.Item('stdafx.cpp');
        var fileConfig = file.FileConfigurations('Debug');
        fileConfig.Tool.UsePrecompiledHeader = 1;
        fileConfig = file.FileConfigurations('Release');
        fileConfig.Tool.UsePrecompiledHeader = 1;

        var fileConfig64 = file.FileConfigurations('Debug|x64');
        if (fileConfig64 != null) {
            fileConfig64.Tool.UsePrecompiledHeader = 1;
            fileConfig64 = file.FileConfigurations('Release|x64');
            fileConfig64.Tool.UsePrecompiledHeader = 1;
        }

        var outfiles = ".\\res\\soui_res.rc2;";
        var cmdline = '';
        var cmd7z = '';
        var cmdcopyres = 'xcopy "%(RootDir)%(Directory)*.*" "$(TargetDir)uires" /e/y/i';
        var psw = wizard.FindSymbol("ZIP_PSW");
        var ResLoadType = wizard.FindSymbol('ResLoaderType');

        //指定uires.idx的编译命令
        var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');
        var DirFor7z;
        var outFile = '';
        if (ResLoadType == 1) {
            outFile = 'uires.zip';
        }
        else if (ResLoadType == 2) {
            outFile = 'uires.7z';
        }
        if (WizardVersion >= 10.0) {
            cmdline = '"$(SOUIPATH)\\tools\\uiresbuilder.exe" -i "%(FullPath)" -p uires -r .\\res\\soui_res.rc2 -h .\\res\\resource.h idtable';
            DirFor7z = '"$(TargetDir)' + outFile + '" "%(RootDir)%(Directory)*"';
        }
        else {
            cmdline = '"$(SOUIPATH)\\tools\\uiresbuilder.exe" -i "$(InputPath)" -p uires -r .\\res\\soui_res.rc2 -h .\\res\\resource.h idtable';
            DirFor7z = '"$(TargetDir)' + outFile + '" "$(InputDir)*"';
        }

        if (ResLoadType == 1) {
            cmd7z = '"$(SOUIPATH)\\tools\\7z.exe" a -tzip ' + DirFor7z;
        }
        else if (ResLoadType == 2) {
            cmd7z = '"$(SOUIPATH)\\tools\\7z.exe" a ' + DirFor7z;
        }

        if (psw != null && psw.length != 0) {
            if (ResLoadType == 1) {
                cmd7z += ' -p"' + psw + '"';
            }
            else if (ResLoadType == 2) {
                cmd7z += ' -p"' + psw + '"' + " -mhe";
            }
        }

        //var ResLoadType = wizard.FindSymbol('ResLoaderType');
        var file = files.Item('uires.idx');
        var fileConfig = file.FileConfigurations('Debug');
        buildTool = fileConfig.Tool;
        buildTool.CommandLine = cmdline;
        buildTool.Description = 'Building SoUI Resource';
        buildTool.Outputs = outfiles;
        fileConfig = file.FileConfigurations('Release');
        buildTool = fileConfig.Tool;
        if (ResLoadType == 0) {
            buildTool.CommandLine = cmdline;
        }
        else if ((ResLoadType == 1) || (ResLoadType == 2)) {
            buildTool.CommandLine = cmdline + "\r\n" + cmd7z;
        }
        else if (ResLoadType == 3) {
            buildTool.CommandLine = cmdline + "\r\n" + cmdcopyres;
        }
        buildTool.Description = 'Building SoUI Resource';
        buildTool.Outputs = outfiles;

        var fileConfig64 = file.FileConfigurations('Debug|x64');
        if (fileConfig64 != null) {
            buildTool64 = fileConfig64.Tool;
            buildTool64.CommandLine = cmdline;
            buildTool64.Description = 'Building SoUI Resource';
            buildTool64.Outputs = outfiles;
            fileConfig64 = file.FileConfigurations('Release|x64');
            buildTool64 = fileConfig64.Tool;
            if (ResLoadType == 0) {
                buildTool.CommandLine = cmdline;
            }
            else if ((ResLoadType == 1) || (ResLoadType == 2)) {
                buildTool.CommandLine = cmdline + "\r\n" + cmd7z;
            }
            else if (ResLoadType == 3) {
                buildTool.CommandLine = cmdline + "\r\n" + cmdcopyres;
            }
            buildTool64.Description = 'Building SoUI Resource';
            buildTool64.Outputs = outfiles;
        }

    }
    catch (e) {
        throw e;
    }
}


// SIG // Begin signature block
// SIG // MIIXPgYJKoZIhvcNAQcCoIIXLzCCFysCAQExCzAJBgUr
// SIG // DgMCGgUAMGcGCisGAQQBgjcCAQSgWTBXMDIGCisGAQQB
// SIG // gjcCAR4wJAIBAQQQEODJBs441BGiowAQS9NQkAIBAAIB
// SIG // AAIBAAIBAAIBADAhMAkGBSsOAwIaBQAEFIJiNUQe05dS
// SIG // lo9zJN22YD4CiqEhoIISMTCCBGAwggNMoAMCAQICCi6r
// SIG // EdxQ/1ydy8AwCQYFKw4DAh0FADBwMSswKQYDVQQLEyJD
// SIG // b3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAu
// SIG // MR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24x
// SIG // ITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0
// SIG // eTAeFw0wNzA4MjIyMjMxMDJaFw0xMjA4MjUwNzAwMDBa
// SIG // MHkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
// SIG // dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
// SIG // aWNyb3NvZnQgQ29ycG9yYXRpb24xIzAhBgNVBAMTGk1p
// SIG // Y3Jvc29mdCBDb2RlIFNpZ25pbmcgUENBMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt3l91l2zRTmo
// SIG // NKwx2vklNUl3wPsfnsdFce/RRujUjMNrTFJi9JkCw03Y
// SIG // SWwvJD5lv84jtwtIt3913UW9qo8OUMUlK/Kg5w0jH9FB
// SIG // JPpimc8ZRaWTSh+ZzbMvIsNKLXxv2RUeO4w5EDndvSn0
// SIG // ZjstATL//idIprVsAYec+7qyY3+C+VyggYSFjrDyuJSj
// SIG // zzimUIUXJ4dO3TD2AD30xvk9gb6G7Ww5py409rQurwp9
// SIG // YpF4ZpyYcw2Gr/LE8yC5TxKNY8ss2TJFGe67SpY7UFMY
// SIG // zmZReaqth8hWPp+CUIhuBbE1wXskvVJmPZlOzCt+M26E
// SIG // RwbRntBKhgJuhgCkwIffUwIDAQABo4H6MIH3MBMGA1Ud
// SIG // JQQMMAoGCCsGAQUFBwMDMIGiBgNVHQEEgZowgZeAEFvQ
// SIG // cO9pcp4jUX4Usk2O/8uhcjBwMSswKQYDVQQLEyJDb3B5
// SIG // cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0IENvcnAuMR4w
// SIG // HAYDVQQLExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xITAf
// SIG // BgNVBAMTGE1pY3Jvc29mdCBSb290IEF1dGhvcml0eYIP
// SIG // AMEAizw8iBHRPvZj7N9AMA8GA1UdEwEB/wQFMAMBAf8w
// SIG // HQYDVR0OBBYEFMwdznYAcFuv8drETppRRC6jRGPwMAsG
// SIG // A1UdDwQEAwIBhjAJBgUrDgMCHQUAA4IBAQB7q65+Siby
// SIG // zrxOdKJYJ3QqdbOG/atMlHgATenK6xjcacUOonzzAkPG
// SIG // yofM+FPMwp+9Vm/wY0SpRADulsia1Ry4C58ZDZTX2h6t
// SIG // KX3v7aZzrI/eOY49mGq8OG3SiK8j/d/p1mkJkYi9/uEA
// SIG // uzTz93z5EBIuBesplpNCayhxtziP4AcNyV1ozb2AQWtm
// SIG // qLu3u440yvIDEHx69dLgQt97/uHhrP7239UNs3DWkuNP
// SIG // tjiifC3UPds0C2I3Ap+BaiOJ9lxjj7BauznXYIxVhBoz
// SIG // 9TuYoIIMol+Lsyy3oaXLq9ogtr8wGYUgFA0qvFL0QeBe
// SIG // MOOSKGmHwXDi86erzoBCcnYOMIIEejCCA2KgAwIBAgIK
// SIG // YQHPPgAAAAAADzANBgkqhkiG9w0BAQUFADB5MQswCQYD
// SIG // VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
// SIG // A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
// SIG // IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3NvZnQg
// SIG // Q29kZSBTaWduaW5nIFBDQTAeFw0wOTEyMDcyMjQwMjla
// SIG // Fw0xMTAzMDcyMjQwMjlaMIGDMQswCQYDVQQGEwJVUzET
// SIG // MBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
// SIG // bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0
// SIG // aW9uMQ0wCwYDVQQLEwRNT1BSMR4wHAYDVQQDExVNaWNy
// SIG // b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEB
// SIG // AQUAA4IBDwAwggEKAoIBAQC9MIn7RXKoU2ueiU8AI8C+
// SIG // 1B09sVlAOPNzkYIm5pYSAFPZHIIOPM4du733Qo2X1Pw4
// SIG // GuS5+ePs02EDv6DT1nVNXEap7V7w0uJpWxpz6rMcjQTN
// SIG // KUSgZFkvHphdbserGDmCZcSnvKt1iBnqh5cUJrN/Jnak
// SIG // 1Dg5hOOzJtUY+Svp0skWWlQh8peNh4Yp/vRJLOaL+AQ/
// SIG // fc3NlpKGDXED4tD+DEI1/9e4P92ORQp99tdLrVvwdnId
// SIG // dyN9iTXEHF2yUANLR20Hp1WImAaApoGtVE7Ygdb6v0LA
// SIG // Mb5VDZnVU0kSMOvlpYh8XsR6WhSHCLQ3aaDrMiSMCOv5
// SIG // 1BS64PzN6qQVAgMBAAGjgfgwgfUwEwYDVR0lBAwwCgYI
// SIG // KwYBBQUHAwMwHQYDVR0OBBYEFDh4BXPIGzKbX5KGVa+J
// SIG // usaZsXSOMA4GA1UdDwEB/wQEAwIHgDAfBgNVHSMEGDAW
// SIG // gBTMHc52AHBbr/HaxE6aUUQuo0Rj8DBEBgNVHR8EPTA7
// SIG // MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20v
// SIG // cGtpL2NybC9wcm9kdWN0cy9DU1BDQS5jcmwwSAYIKwYB
// SIG // BQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8vd3d3
// SIG // Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL0NTUENBLmNy
// SIG // dDANBgkqhkiG9w0BAQUFAAOCAQEAKAODqxMN8f4Rb0J2
// SIG // 2EOruMZC+iRlNK51sHEwjpa2g/py5P7NN+c6cJhRIA66
// SIG // cbTJ9NXkiugocHPV7eHCe+7xVjRagILrENdyA+oSTuzd
// SIG // DYx7RE8MYXX9bpwH3c4rWhgNObBg/dr/BKoCo9j6jqO7
// SIG // vcFqVDsxX+QsbsvxTSoc8h52e4avxofWsSrtrMwOwOSf
// SIG // f+jP6IRyVIIYbirInpW0Gh7Bb5PbYqbBS2utye09kuOy
// SIG // L6t6dzlnagB7gp0DEN5jlUkmQt6VIsGHC9AUo1/cczJy
// SIG // Nh7/yCnFJFJPZkjJHR2pxSY5aVBOp+zCBmwuchvxIdpt
// SIG // JEiAgRVAfJ/MdDhKTzCCBJ0wggOFoAMCAQICEGoLmU/A
// SIG // ACWrEdtFH1h6Z6IwDQYJKoZIhvcNAQEFBQAwcDErMCkG
// SIG // A1UECxMiQ29weXJpZ2h0IChjKSAxOTk3IE1pY3Jvc29m
// SIG // dCBDb3JwLjEeMBwGA1UECxMVTWljcm9zb2Z0IENvcnBv
// SIG // cmF0aW9uMSEwHwYDVQQDExhNaWNyb3NvZnQgUm9vdCBB
// SIG // dXRob3JpdHkwHhcNMDYwOTE2MDEwNDQ3WhcNMTkwOTE1
// SIG // MDcwMDAwWjB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMK
// SIG // V2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwG
// SIG // A1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYD
// SIG // VQQDExpNaWNyb3NvZnQgVGltZXN0YW1waW5nIFBDQTCC
// SIG // ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANw3
// SIG // bvuvyEJKcRjIzkg+U8D6qxS6LDK7Ek9SyIPtPjPZSTGS
// SIG // KLaRZOAfUIS6wkvRfwX473W+i8eo1a5pcGZ4J2botrfv
// SIG // hbnN7qr9EqQLWSIpL89A2VYEG3a1bWRtSlTb3fHev5+D
// SIG // x4Dff0wCN5T1wJ4IVh5oR83ZwHZcL322JQS0VltqHGP/
// SIG // gHw87tUEJU05d3QHXcJc2IY3LHXJDuoeOQl8dv6dbG56
// SIG // 4Ow+j5eecQ5fKk8YYmAyntKDTisiXGhFi94vhBBQsvm1
// SIG // Go1s7iWbE/jLENeFDvSCdnM2xpV6osxgBuwFsIYzt/iU
// SIG // W4RBhFiFlG6wHyxIzG+cQ+Bq6H8mjmsCAwEAAaOCASgw
// SIG // ggEkMBMGA1UdJQQMMAoGCCsGAQUFBwMIMIGiBgNVHQEE
// SIG // gZowgZeAEFvQcO9pcp4jUX4Usk2O/8uhcjBwMSswKQYD
// SIG // VQQLEyJDb3B5cmlnaHQgKGMpIDE5OTcgTWljcm9zb2Z0
// SIG // IENvcnAuMR4wHAYDVQQLExVNaWNyb3NvZnQgQ29ycG9y
// SIG // YXRpb24xITAfBgNVBAMTGE1pY3Jvc29mdCBSb290IEF1
// SIG // dGhvcml0eYIPAMEAizw8iBHRPvZj7N9AMBAGCSsGAQQB
// SIG // gjcVAQQDAgEAMB0GA1UdDgQWBBRv6E4/l7k0q0uGj7yc
// SIG // 6qw7QUPG0DAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMA
// SIG // QTALBgNVHQ8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAN
// SIG // BgkqhkiG9w0BAQUFAAOCAQEAlE0RMcJ8ULsRjqFhBwEO
// SIG // jHBFje9zVL0/CQUt/7hRU4Uc7TmRt6NWC96Mtjsb0fus
// SIG // p8m3sVEhG28IaX5rA6IiRu1stG18IrhG04TzjQ++B4o2
// SIG // wet+6XBdRZ+S0szO3Y7A4b8qzXzsya4y1Ye5y2PENtEY
// SIG // Ib923juasxtzniGI2LS0ElSM9JzCZUqaKCacYIoPO8cT
// SIG // ZXhIu8+tgzpPsGJY3jDp6Tkd44ny2jmB+RMhjGSAYwYE
// SIG // lvKaAkMve0aIuv8C2WX5St7aA3STswVuDMyd3ChhfEjx
// SIG // F5wRITgCHIesBsWWMrjlQMZTPb2pid7oZjeN9CKWnMyw
// SIG // d1RROtZyRLIj9jCCBKowggOSoAMCAQICCmEGlC0AAAAA
// SIG // AAkwDQYJKoZIhvcNAQEFBQAweTELMAkGA1UEBhMCVVMx
// SIG // EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1Jl
// SIG // ZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
// SIG // dGlvbjEjMCEGA1UEAxMaTWljcm9zb2Z0IFRpbWVzdGFt
// SIG // cGluZyBQQ0EwHhcNMDgwNzI1MTkwMjE3WhcNMTMwNzI1
// SIG // MTkxMjE3WjCBszELMAkGA1UEBhMCVVMxEzARBgNVBAgT
// SIG // Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
// SIG // BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjENMAsG
// SIG // A1UECxMETU9QUjEnMCUGA1UECxMebkNpcGhlciBEU0Ug
// SIG // RVNOOjdBODItNjg4QS05RjkyMSUwIwYDVQQDExxNaWNy
// SIG // b3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNlMIIBIjANBgkq
// SIG // hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAlYEKIEIYUXrZ
// SIG // le2b/dyH0fsOjxPqqjcoEnb+TVCrdpcqk0fgqVZpAuWU
// SIG // fk2F239x73UA27tDbPtvrHHwK9F8ks6UF52hxbr5937d
// SIG // YeEtMB6cJi12P+ZGlo6u2Ik32Mzv889bw/xo4PJkj5vo
// SIG // wxL5o76E/NaLzgU9vQF2UCcD+IS3FoaNYL5dKSw8z6X9
// SIG // mFo1HU8WwDjYHmE/PTazVhQVd5U7EPoAsJPiXTerJ7tj
// SIG // LEgUgVXjbOqpK5WNiA5+owCldyQHmCpwA7gqJJCa3sWi
// SIG // Iku/TFkGd1RyQ7A+ZN2ThAhYtv7ph0kJNrOz+DOpfkyi
// SIG // eX8yWSkOnrX14DyeP+xGOwIDAQABo4H4MIH1MB0GA1Ud
// SIG // DgQWBBQolYi/Ajvr2pS6fUYP+sv0fp3/0TAfBgNVHSME
// SIG // GDAWgBRv6E4/l7k0q0uGj7yc6qw7QUPG0DBEBgNVHR8E
// SIG // PTA7MDmgN6A1hjNodHRwOi8vY3JsLm1pY3Jvc29mdC5j
// SIG // b20vcGtpL2NybC9wcm9kdWN0cy90c3BjYS5jcmwwSAYI
// SIG // KwYBBQUHAQEEPDA6MDgGCCsGAQUFBzAChixodHRwOi8v
// SIG // d3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL3RzcGNh
// SIG // LmNydDATBgNVHSUEDDAKBggrBgEFBQcDCDAOBgNVHQ8B
// SIG // Af8EBAMCBsAwDQYJKoZIhvcNAQEFBQADggEBAADurPzi
// SIG // 0ohmyinjWrnNAIJ+F1zFJFkSu6j3a9eH/o3LtXYfGyL2
// SIG // 9+HKtLlBARo3rUg3lnD6zDOnKIy4C7Z0Eyi3s3XhKgni
// SIG // i0/fmD+XtzQSgeoQ3R3cumTPTlA7TIr9Gd0lrtWWh+pL
// SIG // xOXw+UEXXQHrV4h9dnrlb/6HIKyTnIyav18aoBUwJOCi
// SIG // fmGRHSkpw0mQOkODie7e1YPdTyw1O+dBQQGqAAwL8tZJ
// SIG // G85CjXuw8y2NXSnhvo1/kRV2tGD7FCeqbxJjQihYOoo7
// SIG // i0Dkt8XMklccRlZrj8uSTVYFAMr4MEBFTt8ZiL31EPDd
// SIG // Gt8oHrRR8nfgJuO7CYES3B460EUxggR5MIIEdQIBATCB
// SIG // hzB5MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
// SIG // Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMV
// SIG // TWljcm9zb2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpN
// SIG // aWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQQIKYQHPPgAA
// SIG // AAAADzAJBgUrDgMCGgUAoIGkMBkGCSqGSIb3DQEJAzEM
// SIG // BgorBgEEAYI3AgEEMBwGCisGAQQBgjcCAQsxDjAMBgor
// SIG // BgEEAYI3AgEVMCMGCSqGSIb3DQEJBDEWBBQ+jyw0JL53
// SIG // ecLcJJgbfgAgOayUwDBEBgorBgEEAYI3AgEMMTYwNKAa
// SIG // gBgAYwB1AHMAdABvAG0AdwBpAHoALgBqAHOhFoAUaHR0
// SIG // cDovL21pY3Jvc29mdC5jb20wDQYJKoZIhvcNAQEBBQAE
// SIG // ggEAFDvfYSq+/r/7CF8Nw6w+w5s3CE8pJ4tMsTnc7QKa
// SIG // EIhdeJvEpr/TDEnVMZ0FReRJxGp/wSed2W33fvBygsa2
// SIG // iBtduCKUqgR6ERLhtGSQ1G+LvHK8ire4n5LFNvmzeYFV
// SIG // dksEc++0K7p8SLL+9FCfvsDPxmMuYnd1uJmYV73eZy8R
// SIG // 8yhac1+6aYQvkXS4ZEHljzoSNTJGqbOxw08HkdX3zNbZ
// SIG // 1GEnU54kiqiQe12N5BIgoxhCRrCrIaNams2pl7ncAt6W
// SIG // ltSQzoBhMRMFuadTBolQpdfaPvk04LOyWvLU9B3jZd+n
// SIG // hzw7usHXc2h5qObSmud7+n15bKu10/eHZHuVUqGCAh8w
// SIG // ggIbBgkqhkiG9w0BCQYxggIMMIICCAIBATCBhzB5MQsw
// SIG // CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQ
// SIG // MA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
// SIG // b2Z0IENvcnBvcmF0aW9uMSMwIQYDVQQDExpNaWNyb3Nv
// SIG // ZnQgVGltZXN0YW1waW5nIFBDQQIKYQaULQAAAAAACTAH
// SIG // BgUrDgMCGqBdMBgGCSqGSIb3DQEJAzELBgkqhkiG9w0B
// SIG // BwEwHAYJKoZIhvcNAQkFMQ8XDTEwMDUwNDE0MDY1OFow
// SIG // IwYJKoZIhvcNAQkEMRYEFDb6OfV/emfaAnk4p7iKvGVy
// SIG // E33SMA0GCSqGSIb3DQEBBQUABIIBADj0mOcjiCV5Vue7
// SIG // Ly+GTC9hzlzmXVsaO5PQZ/RO+H6LvNMcEVf3AbSWUl0f
// SIG // 1Uw8yOtBvkjOsJLzD5MdPV6fvfoWVbDkNAkod5eZ8yw7
// SIG // 1jujX+GCToR/NXHlE8soBtBfjePrZTuk+uc8M4s4uw0F
// SIG // KqTCFTStbsjW9mQeZJYlX12FQpCWirMbeWzro6oTgXvF
// SIG // QveQ6Qfw0XUfZ99X+gnx0UBzAQ2UTlrCZ1aNylETr124
// SIG // HB9rZDG9t1HqpNHG6LgrlpnNqdVtUVvUa70fzs4Dh8OB
// SIG // rnCnMZDyfV/EE5183Dy4o42V2MJ1f6l1M78jqYyxihme
// SIG // VsYgl5fzMwXZ5uQ/mjc=
// SIG // End signature block
