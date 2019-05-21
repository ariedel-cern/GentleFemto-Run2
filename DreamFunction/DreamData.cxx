/*
 * DreamData.cxx
 *
 *  Created on: 29 Aug 2018
 *      Author: bernhardhohlweger
 */

#include "DreamData.h"
#include "TLegend.h"
#include "TStyle.h"
DreamData::DreamData(const char* particlePair)
    : fName(particlePair),
      fCorrelationFunction(nullptr),
      fCorrelationFunctionSimulation(nullptr),
      fSystematics(nullptr),
      fSysError(nullptr),
      fBaseLine(new TF1(Form("%sBaseLine", particlePair), "pol1", 0, 1000)),
      fXMin(0),
      fXMax(0.5),
      fYMin(0),
      fYMax(0.5),
      fPurgeXaxis(false),
      fInlet(false),
      fXMinZoom(0),
      fXMaxZoom(0.5),
      fYMinZoom(0),
      fYMaxZoom(0.5),
      fXMinInlet(0),
      fXMaxInlet(0.5),
      fYMinInlet(0),
      fYMaxInlet(0.5),
      fXMinLegend(0),
      fXMaxLegend(0.5),
      fYMinLegend(0),
      fYMaxLegend(0.5),
      fUnitConversionData(1),
      fUnitConversionCATS(1),
      fLegendName(),
      fLegendOption(),
      fFemtoModdeled(),
      fFakeGraph() {
  fFillColors = {
    kGray + 1,
    kRed - 10,
    kBlue - 9,
    kGreen - 8,
    kMagenta - 9,
    kOrange - 9,
    kCyan - 3,
    kYellow - 7,
    kBlue + 3
  };
  TColor myColor1;
  fColors = {
    kBlack,         //0
    kRed + 1,//1
    kBlue + 2,//2
    kGreen + 3,//3
    kMagenta - 8,//4
    kOrange - 7,//5
    kCyan + 2,//6
    kYellow + 2,//7
    kWhite,//8
    kGreen - 5,//9
    myColor1.GetColor(255,127,0),//10
    myColor1.GetColor(31,120,180),//11
    myColor1.GetColor(178,223,138),//12
    kBlue + 3//13
  };
  fMarkers = {kFullCircle, kFullSquare, kOpenCircle, kOpenSquare, kOpenDiamond,
    kOpenCross, kFullCross, kFullDiamond, kFullStar, kOpenStar};
  fBaseLine->SetLineStyle(7);
  fBaseLine->SetLineWidth(1);
  fBaseLine->SetLineColor(fColors[0]);

}

DreamData::~DreamData() {
  // TODO Auto-generated destructor stub
}

void DreamData::SetSystematics(TF1* parameters, float errorwidth) {
  if (parameters) {
    if (fCorrelationFunction) {
      int nBinsX = fCorrelationFunction->GetNbinsX();
      float minX = fCorrelationFunction->GetXaxis()->GetXmin();
      float maxX = fCorrelationFunction->GetXaxis()->GetXmax();
      fSystematics = new TH1F(Form("Systematics%s", fName),
                              Form("Systematics%s", fName), nBinsX, minX, maxX);
      for (int iBin = 1; iBin < nBinsX; iBin++) {
        const float x = fCorrelationFunction->GetBinCenter(iBin);
        const float y = fCorrelationFunction->GetBinContent(iBin);
//        std::cout << "x = " << x << '\t' << " y = " << y << '\n'
//                  << " rel error "
//                  << parameters->Eval(x / (float) fUnitConversionData) << '\t'
//                  << " Error = "
//                  << y * parameters->Eval(x / (float) fUnitConversionData)
//                  << std::endl;
        fSystematics->SetBinContent(
            iBin, y * parameters->Eval(x / (float) fUnitConversionData));
      }
      fSystematics->SetLineWidth(2.0);
      fSysError = new TGraphErrors();

      for (int i = 0; i < nBinsX; i++) {
        fSysError->SetPoint(i, fCorrelationFunction->GetBinCenter(i + 1),
                            fCorrelationFunction->GetBinContent(i + 1));
        fSysError->SetPointError(i, errorwidth,
                                 fSystematics->GetBinContent(i + 1));
      }
      TGraph *grFakeSys = new TGraph();
      SetStyleGraph(grFakeSys, 2, 0);
      grFakeSys->SetFillColor(fFillColors[0]);
      grFakeSys->SetLineColor(fFillColors[0]);
      grFakeSys->SetLineWidth(0);
      fFakeGraph.push_back(grFakeSys);
    } else {
      Warning("DreamData", "For %s set the CF before adding the systematics",
              fName);
    }
  } else {
    Warning("DreamData", "Parameters input missing for %s", fName);
  }
  return;
}

void DreamData::SetSystematics(TH1* parameters, float errorwidth) {
  if (parameters) {
    if (fCorrelationFunction) {
      int nBinsX = fCorrelationFunction->GetNbinsX();
      float minX = fCorrelationFunction->GetXaxis()->GetXmin();
      float maxX = fCorrelationFunction->GetXaxis()->GetXmax();
      fSystematics = new TH1F(Form("Systematics%s", fName),
                              Form("Systematics%s", fName), nBinsX, minX, maxX);
      for (int iBin = 1; iBin < nBinsX; iBin++) {
        const float x = fCorrelationFunction->GetBinCenter(iBin);
        const float y = fCorrelationFunction->GetBinContent(iBin);
        fSystematics->SetBinContent(
            iBin, y * parameters->GetBinContent(parameters->FindBin(x)));
      }
      fSystematics->SetLineWidth(2.0);
      fSysError = new TGraphErrors();

      for (int i = 0; i < nBinsX; i++) {
        fSysError->SetPoint(i, fCorrelationFunction->GetBinCenter(i + 1),
                            fCorrelationFunction->GetBinContent(i + 1));
        fSysError->SetPointError(i, errorwidth,
                                 fSystematics->GetBinContent(i + 1));
      }
      TGraph *grFakeSys = new TGraph();
      SetStyleGraph(grFakeSys, 2, 0);
      grFakeSys->SetFillColor(fFillColors[0]);
      grFakeSys->SetLineColor(fFillColors[0]);
      grFakeSys->SetLineWidth(0);
      fFakeGraph.push_back(grFakeSys);
    } else {
      Warning("DreamData", "For %s set the CF before adding the systematics",
              fName);
    }
  } else {
    Warning("DreamData", "Parameters input missing for %s", fName);
  }
  return;
}

void DreamData::FemtoModelFitBands(TGraph *grMedian1, TGraph *grLower,
                                   TGraph *grUpper, int color, int lineStyle,
                                   double lineWidth, int fillStyle,
                                   bool addtoLegend) {
  if (fSystematics) {
    TGraphErrors *grFemtoModel = new TGraphErrors();
    grFemtoModel->SetName(grMedian1->GetName());
    double x, yM1, yLo, yUp;
    int count = 0;
    for (int i = 0; i < grMedian1->GetN(); ++i) {
      grMedian1->GetPoint(i, x, yM1);
      grLower->GetPoint(i, x, yLo);
      grUpper->GetPoint(i, x, yUp);
      std::vector<float> yAll;
      yAll.push_back(yM1);
      yAll.push_back(yLo);
      yAll.push_back(yUp);
      std::sort(yAll.begin(), yAll.end());
      grFemtoModel->SetPoint(count, x / (float) fUnitConversionCATS,
                             (yAll[2] + yAll[0]) / 2.f);
      grFemtoModel->SetPointError(count++, 0,
                                  (yAll[2] + yAll[0]) / 2.f - yAll[0]);
    }
    grFemtoModel->SetLineColor(fColors[color]);
    grFemtoModel->SetFillColor(fColors[color]);
    grFemtoModel->SetLineWidth(lineWidth);
    grFemtoModel->SetLineStyle(lineStyle);
    if (fillStyle > 0)
      grFemtoModel->SetFillStyle(fillStyle);
    fFemtoModdeled.push_back(grFemtoModel);
    if (addtoLegend) {
      TGraph *grFakeModel = new TGraph();
      grFakeModel->SetLineColor(fColors[color]);
      grFakeModel->SetFillColor(fColors[color]);
      grFakeModel->SetLineWidth(lineWidth * 1.8);
      grFakeModel->SetLineStyle(lineStyle);
      if (fillStyle > 0) {
        grFakeModel->SetFillStyle(fillStyle);
      }
      fFakeGraph.push_back(grFakeModel);
    }
  } else {
    Warning("DreamData", "Set Systematics first for %s", fName);
  }
  return;
}

void DreamData::FemtoModelFitBands(TGraphErrors *grFemtoModel, int color,
                                   int lineStyle, double lineWidth,
                                   int fillStyle, bool addtoLegend,
                                   bool useDefaultColors) {
  grFemtoModel->SetLineColor(useDefaultColors ? fColors[color] : color);
  grFemtoModel->SetFillColor(useDefaultColors ? fColors[color] : color);
  grFemtoModel->SetLineWidth(lineWidth);
  grFemtoModel->SetLineStyle(lineStyle);
  if (fillStyle > 0)
    grFemtoModel->SetFillStyle(fillStyle);
  fFemtoModdeled.push_back(grFemtoModel);
  if (addtoLegend) {
    TGraph *grFakeModel = new TGraph();
    grFakeModel->SetLineColor(useDefaultColors ? fColors[color] : color);
    grFakeModel->SetFillColor(useDefaultColors ? fColors[color] : color);
    grFakeModel->SetLineWidth(lineWidth * 1.8);
    grFakeModel->SetLineStyle(lineStyle);
    if (fillStyle > 0) {
      grFakeModel->SetFillStyle(fillStyle);
    }
    fFakeGraph.push_back(grFakeModel);
  }
}

void DreamData::FemtoModelFitBands(TGraphErrors *grFemtoModel, int color,
                                   float colorAlpha, bool addtoLegend) {
  grFemtoModel->SetLineColorAlpha(color, colorAlpha);
  grFemtoModel->SetFillColorAlpha(color, colorAlpha);
  grFemtoModel->SetLineWidth(0);
  fFemtoModdeled.push_back(grFemtoModel);
  if (addtoLegend) {
    TGraph *grFakeModel = new TGraph();
    grFakeModel->SetLineColorAlpha(color, colorAlpha);
    grFakeModel->SetFillColorAlpha(color, colorAlpha);
    grFakeModel->SetLineWidth(5);
    fFakeGraph.push_back(grFakeModel);
  }
}

void DreamData::FemtoModelDeviations(TGraphErrors* grDeviation, int color) {
  SetStyleGraph(grDeviation, 2, color);
  fFemtoDeviation.push_back(grDeviation);
}

void DreamData::SetStyleHisto(TH1 *histo, int marker, int color) {
  histo->GetXaxis()->SetLabelSize(0.045);
  histo->GetXaxis()->SetTitleSize(0.05);
  histo->GetXaxis()->SetLabelOffset(0.01);
  histo->GetXaxis()->SetTitleOffset(1.2);
  histo->GetXaxis()->SetLabelFont(42);
  histo->GetYaxis()->SetLabelSize(0.045);
  histo->GetYaxis()->SetTitleSize(0.05);
  histo->GetYaxis()->SetLabelOffset(0.01);
  histo->GetYaxis()->SetTitleOffset(1.25);
  histo->SetMarkerSize(1.2);
  histo->SetLineWidth(2);
  histo->SetMarkerStyle(fMarkers[marker]);
  histo->SetMarkerColor(fColors[color]);
  histo->SetLineColor(fColors[color]);
}

void DreamData::DrawCorrelationPlot(TPad* c, const int color,
                                    const int systematicsColor) {
  c->cd();
  SetStyleHisto(fCorrelationFunction, 2, color);
  fCorrelationFunction->GetXaxis()->SetRangeUser(fXMin, fXMax);
  fCorrelationFunction->GetYaxis()->SetRangeUser(fYMin, fYMax);
  fSysError->SetLineColor(kWhite);
  fSysError->Draw("Ap");
  fBaseLine->Draw("same");
  TString CFName = fCorrelationFunction->GetName();
  if (CFName.Contains("MeV")) {
    fSysError->SetTitle("; #it{k}* (MeV/#it{c}); #it{C}(#it{k}*)");
  } else {
    fSysError->SetTitle("; #it{k}* (GeV/#it{c}); #it{C}(#it{k}*)");
  }
  fSysError->GetXaxis()->SetRangeUser(fXMin, fXMax);
  fSysError->GetYaxis()->SetRangeUser(fYMin, fYMax);
  if (fPurgeXaxis) {
    fSysError->GetXaxis()->SetTitleSize(0);
    fSysError->GetXaxis()->SetLabelSize(0);
  }
  TLegend *leg = new TLegend(fXMinLegend, fYMinLegend, fXMaxLegend,
                             fYMaxLegend);
//  TLegend *leg = new TLegend(0.5, 0.55, 0.62, 0.875);
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
  leg->SetTextSize(gStyle->GetTextSize() * 0.96);
  int legendCounter = 1;
//  leg->AddEntry(fCorrelationFunction, fLegendName[0], "pe");
  fFakeGraph[0]->SetMarkerStyle(fCorrelationFunction->GetMarkerStyle());
  fFakeGraph[0]->SetMarkerColor(fCorrelationFunction->GetMarkerColor());
  fFakeGraph[0]->SetFillColorAlpha(systematicsColor, 0.4);
  leg->AddEntry(fFakeGraph[0], fLegendName[0], fLegendOption[0]);
//  leg->AddEntry(fBaseLine, "Baseline", "l");
  leg->Draw("same");
  for (auto &it : fFemtoModdeled) {
    if (legendCounter < fFakeGraph.size()) {
      leg->AddEntry(fFakeGraph[legendCounter], fLegendName[legendCounter],
                    fLegendOption[legendCounter]);
    }
    legendCounter++;
  }
  auto it = fFemtoModdeled.rbegin();
  while (it != fFemtoModdeled.rend()) {
    (*it)->Draw("L3 same");
    it++;
  }
  fSysError->SetFillColorAlpha(systematicsColor, 0.4);
  fSysError->Draw("2 same");
  fCorrelationFunction->DrawCopy("pe same");
  leg->Draw("same");
  if (fInlet) {
    DrawInlet(c);
  }
}

void DreamData::DrawInlet(TPad *c) {
  c->cd();
  TPad *inset_pad = new TPad("insert", "insertPad", fXMinInlet, fYMinInlet,
                             fXMaxInlet, fYMaxInlet);
  inset_pad->SetTopMargin(0.01);
  inset_pad->SetRightMargin(0.05);
  inset_pad->SetBottomMargin(0.28);
  inset_pad->SetLeftMargin(0.28);
  inset_pad->SetFillStyle(4000);
  inset_pad->Draw();
  inset_pad->cd();
  TGraphErrors* SysErrCopy = (TGraphErrors*) fSysError->Clone(
      Form("%s_clone", fSysError->GetName()));
  TH1F* CFCopy = (TH1F*) fCorrelationFunction->Clone(
      Form("%s_Cloned", fCorrelationFunction->GetName()));
  SetStyleHisto(CFCopy, 2, 0);
  CFCopy->GetXaxis()->SetRangeUser(fXMinZoom, fXMaxZoom);
  CFCopy->GetYaxis()->SetRangeUser(fYMinZoom, fYMaxZoom);
  SysErrCopy->GetYaxis()->SetLabelSize(0.11);
  SysErrCopy->GetYaxis()->SetNdivisions(203);
  SysErrCopy->GetXaxis()->SetLabelSize(0.11);
  SysErrCopy->GetXaxis()->SetNdivisions(204);
  SysErrCopy->SetTitle("; #it{k}* (MeV/#it{c}); #it{C}(#it{k}*)");
  SysErrCopy->GetXaxis()->SetTitleSize(0.12);
  SysErrCopy->GetXaxis()->SetTitleOffset(0.95);
  SysErrCopy->GetYaxis()->SetTitleSize(0.12);
  SysErrCopy->GetYaxis()->SetTitleOffset(0.75);
  SysErrCopy->SetLineColor(kWhite);
  SysErrCopy->Draw("Ap");
  fBaseLine->Draw("same");
//  SysErrCopy->SetTitle(" ; ; ");
  SysErrCopy->GetXaxis()->SetRangeUser(fXMinZoom, fXMaxZoom);
  SysErrCopy->GetYaxis()->SetRangeUser(fYMinZoom, fYMaxZoom);
  for (auto &it : fFemtoModdeled) {
    it->Draw("L3 same");
  }
  SysErrCopy->SetFillColorAlpha(kBlack, 0.4);
  SysErrCopy->Draw("2 same");
  CFCopy->SetMarkerSize(0.9);
  CFCopy->DrawCopy("pe same");
  return;
}

void DreamData::SetStyleGraph(TGraph *histo, int marker, int color) {
  histo->GetXaxis()->SetLabelSize(0.045);
  histo->GetXaxis()->SetTitleSize(0.05);
  histo->GetXaxis()->SetLabelOffset(0.01);
  histo->GetXaxis()->SetTitleOffset(1.2);
  histo->GetXaxis()->SetLabelFont(42);
  histo->GetYaxis()->SetLabelSize(0.045);
  histo->GetYaxis()->SetTitleSize(0.05);
  histo->GetYaxis()->SetLabelOffset(0.01);
  histo->GetYaxis()->SetTitleOffset(1.25);
  histo->SetMarkerSize(1.4);
  histo->SetLineWidth(2);
  histo->SetMarkerStyle(fMarkers[marker]);
  histo->SetMarkerColor(fColors[color]);
  histo->SetLineColor(fColors[color]);
  histo->SetFillColor(fColors[color]);
}

void DreamData::DrawDeviationPerBin(TPad* c) {
  c->cd();
  TString CFName = fCorrelationFunction->GetName();
  TGraphErrors* tmp = fFemtoDeviation.at(0);
  tmp->GetXaxis()->SetNdivisions(fSysError->GetXaxis()->GetNdivisions());
  tmp->GetXaxis()->SetRangeUser(fXMin, fXMax);
  if (CFName.Contains("MeV")) {
    tmp->SetTitle("; #it{k}* (MeV/#it{c}); #it{C}(#it{k}*)");
  } else {
    tmp->SetTitle("; #it{k}* (GeV/#it{c}); #it{C}(#it{k}*)");
  }
  tmp->Draw("AL3");
//  int nGraphs = fFemtoDeviation.size();
//  for (auto it : fFemtoDeviation) {
//    it->GetXaxis()->SetRangeUser(fXMin,fXMax);
//    it->Draw("L3");
//  }
}
