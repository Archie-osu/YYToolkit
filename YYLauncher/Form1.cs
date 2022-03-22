﻿using System;
using System.Windows.Forms;
using System.Diagnostics;
using System.Net;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;
using Launcher;

namespace Launcher
{
    public partial class MainWindow : Form
    {
        private string sRunnerFilePath; // Fully qualified file path towards selected game executable
        private string sRunnerFileName; // File name of the game executable
        private string sDataFilePath; // Fully qualified file path towards selected data file - empty = default

        public MainWindow()
        {
            InitializeComponent();

            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12; // Fix for Windows 7 downloads

            ContextMenu PluginManagerContextMenu = new ContextMenu();
            PluginManagerContextMenu.MenuItems.Add("Install plugin", new EventHandler(PluginContextMenu_InstallPlugin));
            PluginManagerContextMenu.MenuItems.Add("Uninstall plugin", new EventHandler(PluginContextMenu_UninstallPlugin));
            PluginManagerContextMenu.MenuItems.Add("Enable plugin", new EventHandler(PluginContextMenu_EnablePlugin));
            PluginManagerContextMenu.MenuItems.Add("Disable plugin", new EventHandler(PluginContextMenu_DisablePlugin));
            PluginManagerContextMenu.MenuItems.Add("Refresh list", new EventHandler(PluginContextMenu_Refresh));

            listPlugins.ContextMenu = PluginManagerContextMenu;

            ContextMenu btYYTKLaunchContextMenu = new ContextMenu();
            btYYTKLaunchContextMenu.MenuItems.Add("Launch with custom DLL", new EventHandler(btYYTKLaunchContextMenu_LaunchCustom));

            btYYTKLaunch.ContextMenu = btYYTKLaunchContextMenu;
        }

        private void btRunnerPick_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog fileDialog = Utils.CreateFileDialog("%systemdrive%", "Find the game executable", "Executables|*.exe", 1))
            {
                if (fileDialog.ShowDialog() == DialogResult.OK)
                {
                    sRunnerFilePath = fileDialog.FileName;
                    sRunnerFileName = fileDialog.SafeFileName;

                    Directory.SetCurrentDirectory(fileDialog.FileName.Substring(0, fileDialog.FileName.LastIndexOf("\\")));

                    txtRunner.ResetText();
                    txtRunner.AppendText(sRunnerFilePath);

                    listPlugins.Items.Clear();
                    listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
                }
            }
        }

        private void btYYTKLaunch_Click(object sender, EventArgs e)
        {
            string TempPath = Path.GetTempPath() + "YYToolkit.dll";

            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            using (var Browser = new WebClient())
            {
                try
                {
                    Browser.DownloadFile("https://github.com/Archie-osu/YYToolkit/releases/latest/download/YYToolkit.dll", TempPath);
                }
                catch (System.Exception exception)
                {
                    TempPath = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile) + "\\YYToolkit.dll";
                    try
                    {
                        Browser.DownloadFile("https://github.com/Archie-osu/YYToolkit/releases/latest/download/YYToolkit.dll", TempPath);
                    }
                    catch (System.Exception exception2)
                    {
                        string Exception1Message = "";
                        string Exception2Message = "";

                        while (exception != null)
                        {
                            Exception1Message += exception.Message + "\n";
                            exception = exception.InnerException;
                        }

                        while (exception2 != null)
                        {
                            Exception2Message += exception2.Message + "\n";
                            exception2 = exception2.InnerException;
                        }

                        MessageBox.Show("Couldn't inject YYToolkit.\nPress OK to launch the game without mods.\n\nFirst method: " + Exception1Message + "\nSecond method: " + Exception2Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
            
            if (!cbUsePreloading.Checked)
            {
                Process p;

                if (!string.IsNullOrEmpty(sDataFilePath))
                    p = Process.Start(sRunnerFilePath, "-game \"" + sDataFilePath + "\"");
                else
                    p = Process.Start(sRunnerFilePath);

                while (string.IsNullOrEmpty(p.MainWindowTitle))
                {
                    Thread.Sleep(500);
                    p.Refresh();
                }

                Utils.Inject(p, TempPath);
            }
            else
            {
                Utils.StartPreloaded(sRunnerFilePath, sDataFilePath, TempPath);
            }
        }

        private void btNoModLaunch_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (!string.IsNullOrEmpty(sDataFilePath))
                Process.Start(sRunnerFilePath, "-game \"" + sDataFilePath + "\"");
            else
                Process.Start(sRunnerFilePath);
        }

        private void btResetRunner_Click(object sender, EventArgs e)
        {
            txtRunner.Text = "<none selected>";
            sRunnerFileName = "";
            sRunnerFilePath = "";
        }

        private void btResetData_Click(object sender, EventArgs e)
        {
            txtDataFile.Text = "<use default>";
            sDataFilePath = "";
        }

        private void btDataPick_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (MessageBox.Show("Please note this feature is designed for development purposes.\nProceed anyway?", "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.No)
                return;

            using (OpenFileDialog fileDialog = Utils.CreateFileDialog("%systemdrive%", "Find the data.win file", "GameMaker Data Files|*.win", 1))
            {
                if (fileDialog.ShowDialog() == DialogResult.OK)
                {
                    sDataFilePath = fileDialog.FileName;

                    txtDataFile.ResetText();
                    txtDataFile.AppendText(sDataFilePath);
                }
            }
        }

        private void btOpenPluginFolder_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            Process.Start("explorer", "\"" + Directory.GetCurrentDirectory());
        }

        private void btOpenGitHub_Click(object sender, EventArgs e)
        {
            Process.Start("explorer.exe", $"\"https://github.com/Archie-osu/YYToolkit/releases\"");
        }

        private void PluginContextMenu_InstallPlugin(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            using (OpenFileDialog fileDialog = Utils.CreateFileDialog("%systemdrive%", "Find the plugin file", "YYToolkit plugins|*.dll", 1))
            {
                if (fileDialog.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        if (!Directory.Exists("autoexec"))
                            Directory.CreateDirectory("autoexec");

                        File.Copy(fileDialog.FileName, Directory.GetCurrentDirectory() + "\\autoexec\\" + fileDialog.SafeFileName);
                    }
                    catch (System.Exception Exception)
                    {
                        MessageBox.Show(Exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }

                    // Refresh plugins
                    listPlugins.Items.Clear();
                    listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
                }
            }
        }

        private void PluginContextMenu_UninstallPlugin(object sender, EventArgs e)
        {
            if (!IsReadyToManagePlugins(sRunnerFileName, listPlugins))
                return;

            try
            {
                File.Delete(Directory.GetCurrentDirectory() + "\\autoexec\\" + listPlugins.SelectedItem.ToString());
            }
            catch (System.Exception Exception)
            {
                MessageBox.Show(Exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            // Refresh plugins
            listPlugins.Items.Clear();
            listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
        }

        private void PluginContextMenu_EnablePlugin(object sender, EventArgs e)
        {
            if (!IsReadyToManagePlugins(sRunnerFileName, listPlugins))
                return;

            var PluginName = listPlugins.SelectedItem.ToString();

            if (!PluginName.EndsWith(".dll.disabled"))
            {
                MessageBox.Show("Please pick a disabled plugin.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var PluginPath = Directory.GetCurrentDirectory() + "\\autoexec\\" + PluginName;

            try
            {
                File.Move(PluginPath, PluginPath.Substring(0, PluginPath.LastIndexOf(".disabled")));
            }
            catch (System.Exception Exception)
            {
                MessageBox.Show(Exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            // Refresh plugins
            listPlugins.Items.Clear();
            listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
        }

        private void PluginContextMenu_DisablePlugin(object sender, EventArgs e)
        {
            if (!IsReadyToManagePlugins(sRunnerFileName, listPlugins))
                return;

            var PluginName = listPlugins.SelectedItem.ToString();

            if (PluginName.EndsWith(".dll.disabled"))
            {
                MessageBox.Show("Please pick an enabled plugin.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var PluginPath = Directory.GetCurrentDirectory() + "\\autoexec\\" + PluginName;

            try
            {
                File.Move(PluginPath, PluginPath + ".disabled");
            }
            catch (System.Exception Exception)
            {
                MessageBox.Show(Exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            // Refresh plugins
            listPlugins.Items.Clear();
            listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
        }

        private void PluginContextMenu_Refresh(object sender, EventArgs e)
        {
            listPlugins.Items.Clear();
            listPlugins.Items.AddRange(Utils.GetPluginsFromGameDirectory(Directory.GetCurrentDirectory()));
        }

        private void btOpenData_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (string.IsNullOrEmpty(sDataFilePath))
                Process.Start("explorer.exe", $"\"{Directory.GetCurrentDirectory() + "\\data.win"}\"");
            else
                Process.Start("explorer.exe", $"\"{sDataFilePath}\"");
        }

        private void btYYTKLaunchContextMenu_LaunchCustom(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sRunnerFileName))
            {
                MessageBox.Show("Please select the game executable first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (MessageBox.Show("Please note this feature is designed for development purposes.\nProceed anyway?", "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.No)
                return;

            using (OpenFileDialog fileDialog = Utils.CreateFileDialog("%systemdrive%", "Find the custom DLL", "DLL files|*.dll", 1))
            {
                if (fileDialog.ShowDialog() == DialogResult.OK)
                {
                    if (!cbUsePreloading.Checked)
                    {
                        Process p;

                        if (!string.IsNullOrEmpty(sDataFilePath))
                            p = Process.Start(sRunnerFilePath, "-game \"" + sDataFilePath + "\"");
                        else
                            p = Process.Start(sRunnerFilePath);

                        while (string.IsNullOrEmpty(p.MainWindowTitle))
                        {
                            Thread.Sleep(500);
                            p.Refresh();
                        }

                        Utils.Inject(p, fileDialog.FileName);
                    }
                    else
                    {
                        Utils.StartPreloaded(sRunnerFilePath, sDataFilePath, fileDialog.FileName);
                    }
                }
            }
        }

        private void btOpenUMTGitHub_Click(object sender, EventArgs e)
        {
            Process.Start("explorer.exe", $"\"https://github.com/krzys-h/UndertaleModTool/releases\"");
        }
    }
}